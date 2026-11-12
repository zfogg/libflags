#include <libflags/flags.h>
#include <assert.h>
#include <string.h>
typedef struct { bool verbose; const char *file; } values_t;
int main(void){values_t v={0}; static const flags_option_spec_t o[]={{"verbose",'v',FLAGS_TYPE_BOOL,FLAGS_ARGUMENT_NONE,FLAGS_DASH_LITERAL,0,offsetof(values_t,verbose),sizeof(bool),0},{"file",'f',FLAGS_TYPE_STRING,FLAGS_ARGUMENT_REQUIRED,FLAGS_DASH_STDIN,0,offsetof(values_t,file),sizeof(char*),0}}; flags_schema_t s={o,2,0,0};flags_context_t*c=flags_create(&s,&v,sizeof v);char*a[]={"--file","-","--","-literal","--bad"};flags_parse_result_t r;assert(flags_parse(c,5,a,&r));assert(r.options_ended);assert(r.positional_argc==2);assert(!strcmp(r.positional_argv[0],"-literal"));flags_stream_value_t sv;assert(flags_classify_stream(&o[1],"-",&sv)&&sv.kind==FLAGS_STREAM_STDIN);flags_destroy(c);return 0;}
