#include "phenom/defs.h"
#include "phenom/configuration.h"
#include "phenom/job.h"
#include "phenom/log.h"
#include "phenom/sysutil.h"

int main(int argc, char **argv)
{
  ph_library_init();
  ph_config_load_config_file("config.json");
  ph_nbio_init(0);
  
  ph_debug_console_start("/tmp/phenom-debug-console");
  ph_sched_run();
  
  return 0;
}
