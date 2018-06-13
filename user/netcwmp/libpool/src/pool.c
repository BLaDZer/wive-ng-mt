#include <ctype.h>
#include <cwmp/pool.h>

#ifndef WIN32
#include <stdint.h>
#include <unistd.h>
#endif

#include "../../libcwmp/include/cwmp/log.h"

#define POOL_ALIGNMENT sizeof(unsigned long)
#define pool_align(d, a)     (((d) + (a - 1)) & ~(a - 1))
#define pool_align_ptr(p, a)                                                   \
    (unsigned char *) (((unsigned long) (p) + ((unsigned int) a - 1)) & ~((unsigned long) a - 1))

#ifdef WIN32
#define close_fd    closesocket
#else
#define close_fd    close
#endif

struct pool_cleanup_t
{
    pool_cleanup_handler  handler;
    void                  *data;
    pool_cleanup_t        *next;
};

struct pool_large_t
{
    pool_large_t  *next;
    void          *alloc;
};


struct pool_data_t
{
    unsigned char    *last;
    unsigned char    *end;
    pool_data_t      *next;
};

struct pool_t
{
    const char*         name;
    pool_data_t*        d;
    size_t              size;
    size_t              max;
    pool_data_t         *current;
    pool_chain_t        *chain;
    pool_large_t        *large;
    pool_cleanup_t      *cleanup;
};

struct pool_cleanup_file_t
{
    int                 fd;
    unsigned char       *name;
};


static void *pool_palloc_block(pool_t *pool, size_t size);
static void *pool_palloc_large(pool_t *pool, size_t size);

static void * pool_alloc(size_t size, size_t header_size)
{
    void  *p;
//    cwmp_log_info("------------------pool alloc size: %d------------------", size);
    if (size < header_size)
    {
        cwmp_log_error("pool_alloc size=%d, but header_size=%d. New pool size: %d", size, header_size, size+header_size);
        size += header_size;
    }

    p = malloc(size);

    if (p == NULL)
    {
        cwmp_log_error("malloc() %zu bytes failed", size);
    }

    return p;
}

/*
#if (POOLITE_HAVE_POSIX_MEMALIGN)

void * pool_memalign(size_t alignment, size_t size)
{
    void  *p;

    if (posix_memalign(&p, alignment, size) == -1)
    {
        cwmp_log_error("posix_memalign() %uz bytes aligned to %uz failed",
        size, alignment);
    }
    return p;
}

#elif (POOLITE_HAVE_MEMALIGN)


static void * pool_memalign(size_t alignment, size_t size)
{
    void  *p;

    //p = memalign(alignment, size);
    if (p == NULL)
    {
        cwmp_log_error("memalign() %uz bytes aligned to %uz failed",
            size, alignment);
    }


    return p;
}

#endif
*/

pool_t * pool_create(const char* name, size_t size)
{
    pool_t  *p;

    cwmp_log_debug("~~~ pool create: %s %d", name?name:"", size);

    //FUNCTION_TRACE();

    p = (pool_t*)pool_alloc(size, sizeof(pool_t));
    if (p == NULL)
    {
        return NULL;
    }

    p->name = name;

    p->d = (pool_data_t*)pool_alloc(size, sizeof(pool_data_t));
    if (p->d == NULL)
    {
        return NULL;
    }

    p->d->last = (unsigned char *) p + sizeof(pool_t);
    p->d->end = (unsigned char *) p + size;
    p->d->next = NULL;

    p->size = size;

    size = size - sizeof(pool_t);
    p->max = (size < POOL_MAX_ALLOC_FROM_POOL) ? size : POOL_MAX_ALLOC_FROM_POOL;

    p->current = p->d;
    p->chain = NULL;
    p->large = NULL;
    p->cleanup = NULL;

    return p;
}

void pool_destroy(pool_t *pool)
{
    pool_data_t     *p, *n;
    pool_large_t    *l;
    pool_cleanup_t  *c;
    //FUNCTION_TRACE();
    if(!pool)
    {
        return;
    }

    cwmp_log_debug("~~~ pool destroy: %s %d", pool->name?pool->name:"", pool->size);

    for (c = pool->cleanup; c; c = c->next)
    {
        if (c->handler)
        {
            c->handler(c->data);
        }
    }

    for (l = pool->large; l; l = l->next)
    {
//        cwmp_log_debug("destroy large alloc %p", l->alloc);
        free(l->alloc);
        l->alloc = NULL;
    }


    for (p = pool->d, n = pool->d->next; /* void */; p = n, n = n->next)
    {
        free(p);

        if (n == NULL)
        {
            break;
        }
    }

    free(pool);
}

void pool_clear(pool_t *pool)
{
    pool_data_t          *p, *n;
    pool_large_t    *l;
    pool_cleanup_t  *c;
    //FUNCTION_TRACE();

    for (c = pool->cleanup; c; c = c->next)
    {
        if (c->handler)
        {

            c->handler(c->data);
        }
    }

    for (l = pool->large; l; l = l->next)
    {
//        cwmp_log_debug("pool '%s' clear large alloc %p", pool->name?pool->name:"", l->alloc);

        free(l->alloc);
        l->alloc = NULL;
    }

    for (p = pool->d->next; p; p = n, n = n->next)
    {
        n = p->next;
        //p->d->last = (unsigned char *)p + sizeof(pool_data_t);
        free(p);
        if (n == NULL)
        {
            break;
        }
    }

    pool->d->last = (unsigned char *) pool + sizeof(pool_t);
    pool->d->next = NULL;

    pool->current = pool->d;
    pool->chain = NULL;
    pool->large = NULL;
    pool->cleanup = NULL;

}


void * pool_palloc(pool_t *pool, size_t size)
{
    unsigned char *m;
    pool_data_t    *p;

    if (size <= pool->max)
    {
        p = pool->current;

        while(p)
        {
//            cwmp_log_info("pool %s palloc size(%d)/max(%d) elem=%d", pool->name, size, pool->max, ((char*)p->last) - (char*)p);
            m = pool_align_ptr(p->last, POOL_ALIGNMENT);

            if (p->end > m && (size_t) (p->end - m) >= size)
            {
                p->last = m + size;

                return m;
            }

            p = p->next;

        }

        return pool_palloc_block(pool, size);
    }

    return pool_palloc_large(pool, size);
}

void * pool_pnalloc(pool_t *pool, size_t size)
{
    unsigned char      *m;
    pool_data_t  *p;

    if (size <= pool->max)
    {

        p = pool->current;

        do
        {
            m = p->last;

            if ((size_t) (p->end - m) >= size)
            {
                p->last = m + size;

                return m;
            }

            p = p->next;

        }
        while (p);

        return pool_palloc_block(pool, size);
    }

    return pool_palloc_large(pool, size);
}

static void * pool_palloc_block(pool_t *pool, size_t size)
{
    unsigned char      *m;
    size_t       psize;
    pool_data_t  *p, *newp, *current;

//    cwmp_log_debug("~~~~ pool '%s' alloc memory block: %d", pool->name?pool->name:"", size);

    psize = (size_t) (pool->d->end - (unsigned char *) pool);

    m = (unsigned char*) pool_alloc(psize, sizeof(pool_data_t));
    if (m == NULL)
    {
        return NULL;
    }

    newp = (pool_data_t *) m;

    newp->end = m + psize;
    newp->next = NULL;

    m += sizeof(pool_data_t);
    newp->last = m + size;

    current = pool->current;

    for (p = current; p && p->next; p = p->next)
    {
        if ((size_t) (p->end - p->last) < POOL_ALIGNMENT)
        {
            current = p->next;
        }
    }

    p->next = newp;
    pool->current = current ? current : newp;

    return m;
}

static void * pool_palloc_large(pool_t *pool, size_t size)
{
    void              *p;
    pool_large_t  *large;

    //FUNCTION_TRACE();

    p = pool_alloc(size, 1);
    if (p == NULL)
    {
        cwmp_log_error("unable to alloc large memory block: %d", size);
        return NULL;
    }

    large = pool_palloc(pool, sizeof(pool_large_t));
    if (large == NULL)
    {
        //        cwmp_log_debug("large is null, free p %p", p);
        free(p);
        return NULL;
    }


    if (pool->name && strcmp(pool->name,"cwmp_model_load_xml") != 0)
    {
        cwmp_log_debug("~~~~ pool '%s' alloc large memory block: %d, %p", pool->name?pool->name:"", size, p);
    }

    large->alloc = p;
    large->next = pool->large;
    pool->large = large;

    return p;
}

/*
    void *
pool_pmemalign(pool_t *pool, size_t size, size_t alignment)
{
    //FIX-ME
    void              *p;
    pool_large_t  *large;

//    p = pool_memalign(alignment, size);
//      if (p == NULL) {
//      return NULL;
//      }
     

    //FUNCTION_TRACE();

    large = pool_palloc(pool, sizeof(pool_large_t));
    if (large == NULL)
    {
        free(p);
        return NULL;
    }

    large->alloc = p;
    large->next = pool->large;
    pool->large = large;

    return p;
}
*/

int pool_pfree(pool_t *pool, void *p)
{
    pool_large_t  *l;

//    cwmp_log_debug("~~~~ pool '%s' pfree: %p", pool->name?pool->name:"",p);

    for (l = pool->large; l; l = l->next)
    {
        if (p == l->alloc)
        {
            free(l->alloc);
            l->alloc = NULL;

            return 0;
        }
    }

    return -1;
}


void * pool_pcalloc(pool_t *pool, size_t size)
{
    void *p;

    p = pool_palloc(pool, size);
    if (p)
    {
        memset(p,0, size);
    }

    return p;
}


pool_cleanup_t * pool_cleanup_palloc(pool_t *p, size_t size)
{
    pool_cleanup_t  *c;

    c = pool_palloc(p, sizeof(pool_cleanup_t));
    if (c == NULL)
    {
        return NULL;
    }

    if (size)
    {
        c->data = pool_palloc(p, size);
        if (c->data == NULL)
        {
            return NULL;
        }

    }
    else
    {
        c->data = NULL;
    }

    c->handler = NULL;
    c->next = p->cleanup;

    p->cleanup = c;

    return c;
}

void * pool_cleanup_add(pool_t * pool, pool_cleanup_handler handler, void * data)
{
    pool_cleanup_t  *c;

    c = pool_palloc(pool, sizeof(pool_cleanup_t));
    if (c == NULL)
    {
        return NULL;
    }

    c->handler = handler;
    c->data = data;
    c->next = pool->cleanup;

    pool->cleanup = c;

    return c;
}

void pool_cleanup_file(void *data)
{
    pool_cleanup_file_t  *c = data;

    if (close_fd(c->fd) == -1)
    {
        //        cwmp_log_error("close() \"%s\" failed", c->name);
    }
}

void pool_delete_file(void *data)
{
    pool_cleanup_file_t  *c = data;

    int err;

    if (unlink((char*)c->name) == -1)
    {
        err = errno;

        if (err != ENOENT)
        {
            //            cwmp_log_error("delete() \"%s\" failed", c->name);
        }
    }

    if (close_fd(c->fd) == -1)
    {
        //        cwmp_log_error("close() \"%s\" failed", c->name);
    }
}

/*

static void * pool_get_cached_block(size_t size)
{
    void                      *p;
    pool_cached_block_slot_t  *slot;

    if (pool_cycle->cache == NULL)
    {
        return NULL;
    }

    slot = &pool_cycle->cache[(size + pool_pagesize - 1) / pool_pagesize];

    slot->tries++;

    if (slot->number)
    {
        p = slot->block;
        slot->block = slot->block->next;
        slot->number--;
        return p;
    }

    return NULL;
}

*/

void * pool_prealloc(pool_t * pool, void * ptr, size_t oldsize, size_t newsize)
{
    void * p = pool_pcalloc(pool, newsize);
    if (!p)
    {
        return NULL;
    }
    memcpy(p, ptr, oldsize);
    pool_pfree(pool, ptr);
    return p;
}

void * pool_pmemdup(pool_t * pool, const  void * ptr, size_t size)
{

    void * p = pool_pcalloc(pool, size);
    if (!p)
    {
        return NULL;
    }
    memcpy(p, ptr, size);
    return p;
}

char * pool_pstrdup(pool_t * pool, const  void * ptr)
{
    size_t s;
    if (!ptr || ((char*)ptr)[0] == '\0')
    {
        return NULL;
    }
    s = strlen(ptr);
    return (char *)pool_pmemdup(pool, ptr, s+1);
}

char * pool_pstrdup_lower(pool_t *pool, const char *ptr)
{
    char *copy = pool_pstrdup (pool, ptr);
    char *p = copy;

    //FUNCTION_TRACE();
    for (; *p; p++)
        *p = tolower (*p);
    return copy;
}

