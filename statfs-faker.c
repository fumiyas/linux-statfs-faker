#define _LARGEFILE64_SOURCE

#ifdef MAIN
#include <stdio.h>
#endif

#include <sys/vfs.h>
#include <linux/magic.h>

#include <stdlib.h>
#include <errno.h>
#include <link.h>
#include <string.h>
#include <pthread.h>

#ifdef __mips__
#define OFFSET(map, x) ((x) + ((map)->l_addr))
#else
#define OFFSET(map, x) (x)
#endif

int (*statfs_orig)(
  const char *path,
  struct statfs *buf
);
int (*fstatfs_orig)(
  int fd,
  struct statfs *buf
);
int (*statfs64_orig)(
  const char *path,
  struct statfs64 *buf
);
int (*fstatfs64_orig)(
  int fd,
  struct statfs64 *buf
);

static __fsword_t fake_f_type = -1;

static pthread_once_t statfs_init_once = PTHREAD_ONCE_INIT;

static void statfs_orig_init(void)
{
  struct link_map *map = _r_debug.r_map;

  if (!map) {
    // FIXME
    return;
  }

  for (; map; map = map->l_next) {
    if (!strstr(map->l_name, "/libc.so")) {
      continue;
    }

    ElfW(Dyn) *dyn;
    ElfW(Sym) *symtab = NULL;
    char const *strtab = NULL;
    unsigned int symnum = 0;

    for (dyn = map->l_ld; dyn->d_tag != DT_NULL; ++dyn) {
      if (dyn->d_tag == DT_SYMTAB) {
	symtab = (ElfW(Sym) *)OFFSET(map, dyn->d_un.d_ptr);
      }
      else if (dyn->d_tag == DT_STRTAB) {
	strtab = (char const *)OFFSET(map, dyn->d_un.d_ptr);
      }
      else if (dyn->d_tag == DT_HASH) {
	symnum = ((unsigned int *)OFFSET(map, dyn->d_un.d_ptr))[1];
      }
    }

    if (!symtab || !strtab || !symnum) {
      // FIXME
      return;
    }

    for (; symnum; symtab++, symnum--) {
      char const *name = strtab + symtab->st_name;
      void *value = (void *)symtab->st_value + map->l_addr;

      if (!strcmp(name, "statfs")) {
	statfs_orig = value;
#ifdef MAIN
	printf("statfs() found: %p\n", value);
#endif
      }
      else if (!strcmp(name, "fstatfs")) {
	fstatfs_orig = value;
#ifdef MAIN
	printf("fstatfs() found: %p\n", value);
#endif
      }
      else if (!strcmp(name, "statfs64")) {
	statfs64_orig = value;
#ifdef MAIN
	printf("statfs64() found: %p\n", value);
#endif
      }
      else if (!strcmp(name, "fstatfs64")) {
	fstatfs64_orig = value;
#ifdef MAIN
	printf("fstatfs64() found: %p\n", value);
#endif
      }
    }
  }

  char *fake_f_type_str = getenv("STATFS_FAKER_F_TYPE");
  if (fake_f_type_str) {
    long fake_f_type_rc;
    char *endptr;

    fake_f_type_rc = strtol(fake_f_type_str, &endptr, 0);
    if (*endptr == '\0') {
      fake_f_type = (__fsword_t)fake_f_type_rc;
    }
  }
}

static void statfs_faker(struct statfs *buf)
{
  if (fake_f_type >= 0) {
    buf->f_type = fake_f_type;
  }
}

int statfs(const char *path, struct statfs *buf)
{
  int rc;

  (void)pthread_once(&statfs_init_once, statfs_orig_init);

  if (!statfs_orig) {
    errno = ENOSYS;
    return -1;
  }

  rc = statfs_orig(path, buf);
  if (rc != 0) {
    return rc;
  }

  statfs_faker(buf);

  return rc;
}

int fstatfs(int fd, struct statfs *buf)
{
  int rc;

  (void)pthread_once(&statfs_init_once, statfs_orig_init);

  if (!fstatfs_orig) {
    errno = ENOSYS;
    return -1;
  }

  rc = fstatfs_orig(fd, buf);
  if (rc != 0) {
    return rc;
  }

  statfs_faker(buf);

  return rc;
}

static void statfs64_faker(struct statfs64 *buf)
{
  if (fake_f_type) {
    buf->f_type = fake_f_type;
  }
}

int statfs64(const char *path, struct statfs64 *buf)
{
  int rc;

  (void)pthread_once(&statfs_init_once, statfs_orig_init);

  if (!statfs64_orig) {
    errno = ENOSYS;
    return -1;
  }

  rc = statfs64_orig(path, buf);
  if (rc != 0) {
    return rc;
  }

  statfs64_faker(buf);

  return rc;
}

int fstatfs64(int fd, struct statfs64 *buf)
{
  int rc;

  (void)pthread_once(&statfs_init_once, statfs_orig_init);

  if (!fstatfs64_orig) {
    errno = ENOSYS;
    return -1;
  }

  rc = fstatfs64_orig(fd, buf);
  if (rc != 0) {
    return rc;
  }

  statfs64_faker(buf);

  return rc;
}

#ifdef MAIN
int main(int argc, const char **argv)
{
  int rc;
  const char *path;
  struct statfs64 buf;

  path = argc >= 2 ? argv[1] : "/";
  rc = statfs(path, &buf);
  if (rc != 0) {
    return 1;
  }

  printf("path:   %s\n", path);
  printf("f_type: 0x%lX\n", (long)buf.f_type);

  return rc;
}
#endif
