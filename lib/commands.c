#include <libflags/commands.h>
#include <libplatform/util/string.h>

#include <stdlib.h>
#include <string.h>

/** Private command-tree node. */
struct flags_command {
  flags_command_t *parent;
  char *name;
  char *description;
  flags_command_t **children;
  size_t child_count;
  const flags_option_spec_t **options;
  size_t option_count;
  flags_command_t *default_command;
  struct implicit_command {
    flags_command_t *command;
    char *option_name;
  } *implicit_commands;
  size_t implicit_count;
  int (*handler)(flags_context_t *, void *);
  void *user_data;
};

flags_command_t *flags_command_create(flags_command_t *parent, const char *name, const char *description) {
  if (!name || !*name)
    return NULL;
  flags_command_t *command = calloc(1, sizeof(*command));
  if (!command)
    return NULL;
  command->parent = parent;
  command->name = platform_strdup(name);
  command->description = platform_strdup(description);
  if (!command->name || (description && !command->description)) {
    flags_command_destroy(command);
    return NULL;
  }
  if (parent) {
    flags_command_t **children = realloc(parent->children, (parent->child_count + 1) * sizeof(*children));
    if (!children) {
      flags_command_destroy(command);
      return NULL;
    }
    parent->children = children;
    parent->children[parent->child_count++] = command;
  }
  return command;
}

void flags_command_destroy(flags_command_t *command) {
  if (!command)
    return;
  for (size_t i = 0; i < command->child_count; ++i)
    flags_command_destroy(command->children[i]);
  free(command->children);
  free(command->options);
  for (size_t i = 0; i < command->implicit_count; ++i)
    free(command->implicit_commands[i].option_name);
  free(command->implicit_commands);
  free(command->name);
  free(command->description);
  free(command);
}

bool flags_command_add_option(flags_command_t *command, const flags_option_spec_t *spec) {
  if (!command || !spec || !spec->name)
    return false;
  const flags_option_spec_t **options = realloc(command->options, (command->option_count + 1) * sizeof(*options));
  if (!options)
    return false;
  command->options = options;
  command->options[command->option_count++] = spec;
  return true;
}

bool flags_command_set_handler(flags_command_t *command, int (*handler)(flags_context_t *, void *), void *user_data) {
  if (!command)
    return false;
  command->handler = handler;
  command->user_data = user_data;
  return true;
}

bool flags_command_set_default(flags_command_t *root, flags_command_t *command) {
  if (!root || !command)
    return false;
  for (flags_command_t *cursor = command; cursor; cursor = cursor->parent)
    if (cursor == root) {
      root->default_command = command;
      return true;
    }
  return false;
}

bool flags_command_set_implicit_for_option(flags_command_t *root, flags_command_t *command, const char *option_name) {
  if (!root || !command || !option_name || !*option_name)
    return false;
  for (flags_command_t *cursor = command; cursor; cursor = cursor->parent)
    if (cursor == root) {
      struct implicit_command *entries = realloc(root->implicit_commands, (root->implicit_count + 1) * sizeof(*entries));
      if (!entries)
        return false;
      char *copy = platform_strdup(option_name);
      if (!copy)
        return false;
      root->implicit_commands = entries;
      root->implicit_commands[root->implicit_count++] = (struct implicit_command){command, copy};
      return true;
    }
  return false;
}

static flags_command_t *find_child(flags_command_t *root, const char *name) {
  for (size_t i = 0; i < root->child_count; ++i)
    if (!strcmp(root->children[i]->name, name))
      return root->children[i];
  return NULL;
}

static const flags_option_spec_t *find_option(flags_command_t *command, const char *name, size_t length) {
  for (flags_command_t *cursor = command; cursor; cursor = cursor->parent)
    for (size_t i = 0; i < cursor->option_count; ++i) {
      const flags_option_spec_t *option = cursor->options[i];
      if (strlen(option->name) == length && strncmp(option->name, name, length) == 0)
        return option;
      if (length == 1 && option->short_name == name[0])
        return option;
    }
  return NULL;
}

bool flags_resolve_command_ex(flags_command_t *root, int argc, char **argv, flags_command_resolution_t *result) {
  if (!root || argc < 0 || (argc && !argv) || !result)
    return false;
  *result = (flags_command_resolution_t){.command = root, .command_index = -1, .used_default_command = true, .first_positional_index = -1};
  flags_command_t *command = root;
  bool ended = false;
  for (int i = 1; i < argc; ++i) {
    if (!ended && !strcmp(argv[i], "--")) {
      ended = true;
      continue;
    }
    if (ended) {
      if (result->first_positional_index < 0 && i < argc)
        result->first_positional_index = i;
      break;
    }
    /* A lone '-' is data, never an option or a command-resolution control token. */
    if (argv[i][0] == '-' && argv[i][1] != '\0') {
      const char *name = argv[i] + 1;
      if (*name == '-')
        ++name;
      const char *equals = strchr(name, '=');
      size_t length = equals ? (size_t)(equals - name) : strlen(name);
      const flags_option_spec_t *option = find_option(command, name, length);
      for (size_t j = 0; j < root->implicit_count; ++j)
        if (strlen(root->implicit_commands[j].option_name) == length && !strncmp(root->implicit_commands[j].option_name, name, length)) {
          command = root->implicit_commands[j].command;
          result->used_default_command = false;
          result->used_implicit_command = true;
          break;
        }
      if (option && option->argument_requirement != FLAGS_ARGUMENT_NONE && !equals && i + 1 < argc)
        ++i;
      continue;
    }
    if (result->first_positional_index < 0)
      result->first_positional_index = i;
    flags_command_t *child = find_child(command, argv[i]);
    if (!child)
      break;
    command = child;
    result->command_index = i;
    result->used_default_command = false;
  }
  if (command == root && root->default_command)
    command = root->default_command;
  result->command = command;
  result->options_ended = ended;
  return true;
}

flags_command_t *flags_resolve_command(flags_command_t *root, int argc, char **argv, int *command_index, bool *used_default) {
  flags_command_resolution_t result;
  if (!flags_resolve_command_ex(root, argc, argv, &result))
    return NULL;
  if (command_index)
    *command_index = result.command_index;
  if (used_default)
    *used_default = result.used_default_command;
  return result.command;
}

const char *flags_command_name(const flags_command_t *command) {
  return command ? command->name : NULL;
}

flags_command_t *flags_command_parent(const flags_command_t *command) {
  return command ? command->parent : NULL;
}
