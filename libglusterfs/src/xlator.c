
#include "xlator.h"
#include <dlfcn.h>
#include <netdb.h>
#include "defaults.h"

static void
fill_defaults (struct xlator *xl)
{
  if (!xl->getlayout)
    xl->getlayout = default_getlayout;

  if (!xl->setlayout)
    xl->setlayout = default_setlayout;

  if (!xl->fops->open)
    xl->fops->open = default_open;

  if (!xl->fops->chmod)
    xl->fops->chmod = default_chmod;

}

void
xlator_set_type (struct xlator *xl, 
		 const char *type)
{
  char *name = NULL;
  void *handle = NULL;

  printf ("Attempt to load type %s\n", type);
  asprintf (&name, "%s/%s.so", XLATORDIR, type);
  printf ("Attempt to load file %s\n", name);


  handle = dlopen (name, RTLD_LAZY);

  if (!handle) {
    fprintf (stderr, "dlopen(%s): %s\n", name, dlerror ());
    exit (1);
  }

  if (!(xl->fops = dlsym (handle, "fops"))) {
    fprintf (stderr, "dlsym(fops) on %s\n", dlerror ());
    exit (1);
  }
  if (!(xl->mgmt_ops = dlsym (handle, "mgmt_ops"))) {
    fprintf (stderr, "dlsym(mgmt_ops) on %s\n", dlerror ());
    exit (1);
  }

  if (!(xl->init = dlsym (handle, "init"))) {
    fprintf (stderr, "dlsym(init) on %s\n", dlerror ());
    exit (1);
  }

  if (!(xl->fini = dlsym (handle, "fini"))) {
    fprintf (stderr, "dlsym(fini) on %s\n", dlerror ());
    exit (1);
  }

  if (!(xl->getlayout = dlsym (handle, "getlayout"))) {
    xl->getlayout = default_getlayout;
  }

  if (!(xl->getlayout = dlsym (handle, "setlayout"))) {
    xl->setlayout = default_setlayout;
  }

  fill_defaults (xl);

  free (name);
  return ;
}

in_addr_t
resolve_ip (const char *hostname)
{
  in_addr_t addr;
  struct hostent *h = gethostbyname (hostname);
  if (!h)
    return INADDR_NONE;
  memcpy (&addr, h->h_addr, h->h_length);

  return addr;
}
