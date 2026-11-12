#include <libflags/commands.h>

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
  int (*handler)(flags_context_t *, void *);
  void *user_data;
};

static char *command_strdup(const char *text) {
  if (!text)
    return NULL;
  size_t length = strlen(text) + 1;
  char *copy = malloc(length);
  if (copy)
    memcpy(copy, text, length);
  return copy;
}

flags_command_t *flags_command_create(flags_command_t *parent, const char *name, const char *description) {
  if (!name || !*name)
    return NULL;
  flags_command_t *command = calloc(1, sizeof(*command));
  if (!command)
    return NULL;
  command->parent = parent;
  command->name = command_strdup(name);
  command->description = command_strdup(description);
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

static flags_command_t *find_child(flags_command_t *root, const char *name) {
  for (size_t i = 0; i < root->child_count; ++i)
    if (!strcmp(root->children[i]->name, name))
      return root->children[i];
  return NULL;
}

flags_command_t *flags_resolve_command(flags_command_t *root, int argc, char **argv, int *command_index, bool *used_default) {
  if (!root || argc < 0 || (argc && !argv))
    return NULL;
  if (command_index)
    *command_index = -1;
  if (used_default)
    *used_default = true;
  flags_command_t *command = root;
  bool ended = false;
  for (int i = 0; i < argc; ++i) {
    if (!ended && !strcmp(argv[i], "--")) {
      ended = true;
      continue;
    }
    if (ended)
      break;
    if (!ended && argv[i][0] == '-') {
      if (argv[i][1] == '-')
        continue;
      continue;
    }
    flags_command_t *child = find_child(command, argv[i]);
    if (!child)
      break;
    command = child;
    if (command_index)
      *command_index = i;
    if (used_default)
      *used_default = false;
  }
  if (command == root && root->default_command)
    command = root->default_command;
  return command;
}

const char *flags_command_name(const flags_command_t *command) {
  return command ? command->name : NULL;
}

flags_command_t *flags_command_parent(const flags_command_t *command) {
  return command ? command->parent : NULL;
}
