/***************************************************************************
 *   Copyright (C) 2015 by OmanTek                                         *
 *   Author Kyle Hayes  kylehayes@omantek.com                              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

/**************************************************************************
 * CHANGE LOG                                                             *
 *                                                                        *
 * 2012-07-05  KRH - Created file.                                        *
 *                                                                        *
 * 2012-07-25  KRH - Updates for new thread API.                          *
 *                                                                        *
 **************************************************************************/


/***************************************************************************
 ******************************* WINDOWS ***********************************
 **************************************************************************/

#include <platform.h>

#define _WINSOCKAPI_
#include <windows.h>
#include <tchar.h>
#include <strsafe.h>
#include <io.h>
#include <Winsock2.h>
#include <Ws2tcpip.h>
#include <string.h>
#include <stdlib.h>
#include <winnt.h>
#include <errno.h>
#include <math.h>
#include <process.h>
#include <time.h>
#include <stdio.h>

#include <lib/libplctag.h>
#include <util/debug.h>


/*#ifdef __cplusplus
extern "C"
{
#endif
*/

/*#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>*/








/***************************************************************************
 ******************************* Memory ************************************
 **************************************************************************/



/*
 * mem_alloc
 *
 * This is a wrapper around the platform's memory allocation routine.
 * It will zero out memory before returning it.
 *
 * It will return NULL on failure.
 */
extern void *mem_alloc(int size)
{
    void *res = calloc(size, 1);

    return res;
}



/*
 * mem_free
 *
 * Free the allocated memory passed in.  If the passed pointer is
 * null, do nothing.
 */
extern void mem_free(const void *mem)
{
    if(mem) {
        free((void *)mem);
    }
}




/*
 * mem_set
 *
 * set memory to the passed argument.
 */
extern void mem_set(void *d1, int c, int size)
{
    memset(d1, c, size);
}





/*
 * mem_copy
 *
 * copy memory from one pointer to another for the passed number of bytes.
 */
extern void mem_copy(void *d1, void *d2, int size)
{
    memcpy(d1, d2, size);
}





/*
 * mem_move
 *
 * move memory from one pointer to another for the passed number of bytes.
 */
extern void mem_move(void *dest, void *src, int size)
{
    memmove(dest, src, size);
}





int mem_cmp(void *src1, int src1_size, void *src2, int src2_size)
{
    /* short circuit the comparison if the blocks are different lengths */
    if(src1_size != src2_size) {
        return (src1_size - src2_size);
    }

    return memcmp(src1, src2, src1_size);
}





/***************************************************************************
 ******************************* Strings ***********************************
 **************************************************************************/


/*
 * str_cmp
 *
 * Return -1, 0, or 1 depending on whether the first string is "less" than the
 * second, the same as the second, or "greater" than the second.  This routine
 * just passes through to POSIX strcmp.
 */
extern int str_cmp(const char *first, const char *second)
{
    return strcmp(first, second);
}




/*
 * str_cmp_i
 *
 * Returns -1, 0, or 1 depending on whether the first string is "less" than the
 * second, the same as the second, or "greater" than the second.  The comparison
 * is done case insensitive.
 *
 * It just passes this through to Windows stricmp.
 */
extern int str_cmp_i(const char *first, const char *second)
{
    return _stricmp(first,second);
}



/*
 * str_copy
 *
 * Returns
 */
extern int str_copy(char *dst, int dst_size, const char *src)
{
    if (!src) {
        return PLCTAG_ERR_NULL_PTR;
    }

    /* FIXME - if there is not enough room, truncate the string. */
    strncpy_s(dst, dst_size, src, _TRUNCATE);

    return 0;
}


/*
 * str_length
 *
 * Return the length of the string.  If a null pointer is passed, return
 * null.
 */
extern int str_length(const char *str)
{
    if(!str) {
        return 0;
    }

    return (int)strlen(str);
}




/*
 * str_dup
 *
 * Copy the passed string and return a pointer to the copy.
 * The caller is responsible for freeing the memory.
 */
extern char *str_dup(const char *str)
{
    if(!str) {
        return NULL;
    }

    return _strdup(str);
}



/*
 * str_to_int
 *
 * Convert the characters in the passed string into
 * an int.  Return an int in integer in the passed
 * pointer and a status from the function.
 */
extern int str_to_int(const char *str, int *val)
{
    char *endptr;
    long int tmp_val;

    tmp_val = strtol(str,&endptr,0);

    if (errno == ERANGE && (tmp_val == LONG_MAX || tmp_val == LONG_MIN)) {
        /*pdebug("strtol returned %ld with errno %d",tmp_val, errno);*/
        return -1;
    }

    if (endptr == str) {
        return -1;
    }

    /* FIXME - this will truncate long values. */
    *val = (int)tmp_val;

    return 0;
}


extern int str_to_float(const char *str, float *val)
{
    char *endptr;
    double tmp_val_d;
    float tmp_val;

    /* Windows does not have strtof() */
    tmp_val_d = strtod(str,&endptr);

    if (errno == ERANGE && (tmp_val_d == HUGE_VAL || tmp_val_d == -HUGE_VAL || tmp_val_d == (double)0.0)) {
        return -1;
    }

    if (endptr == str) {
        return -1;
    }

    /* FIXME - this will truncate long values. */
    tmp_val = (float)tmp_val_d;
    *val = tmp_val;

    return 0;
}


extern char **str_split(const char *str, const char *sep)
{
    int sub_str_count=0;
    int size = 0;
    const char *sub;
    const char *tmp;
    char **res;

    /* first, count the sub strings */
    tmp = str;
    sub = strstr(tmp,sep);

    while(sub && *sub) {
        /* separator could be at the front, ignore that. */
        if(sub != tmp) {
            sub_str_count++;
        }

        tmp = sub + str_length(sep);
        sub = strstr(tmp,sep);
    }

    if(tmp && *tmp && (!sub || !*sub))
        sub_str_count++;

    /* calculate total size for string plus pointers */
    size = sizeof(char *)*(sub_str_count+1)+str_length(str)+1;

    /* allocate enough memory */
    res = (char**)mem_alloc(size);
    if(!res)
        return NULL;

    /* calculate the beginning of the string */
    tmp = (char *)res + sizeof(char *)*(sub_str_count+1);

    /* copy the string into the new buffer past the first part with the array of char pointers. */
    str_copy((char *)tmp, (int)(size - ((char*)tmp - (char*)res)), str);

    /* set up the pointers */
    sub_str_count=0;
    sub = strstr(tmp,sep);
    while(sub && *sub) {
        /* separator could be at the front, ignore that. */
        if(sub != tmp) {
            /* store the pointer */
            res[sub_str_count] = (char *)tmp;

            sub_str_count++;
        }

        /* zero out the separator chars */
        mem_set((char*)sub,0,str_length(sep));

        /* point past the separator (now zero) */
        tmp = sub + str_length(sep);

        /* find the next separator */
        sub = strstr(tmp,sep);
    }

    /* if there is a chunk at the end, store it. */
    if(tmp && *tmp && (!sub || !*sub)) {
        res[sub_str_count] = (char*)tmp;
    }

    return res;
}


char *str_concat_impl(int num_args, ...)
{
    va_list arg_list;
    int total_length = 0;
    char *result = NULL;
    char *tmp = NULL;

    /* first loop to find the length */
    va_start(arg_list, num_args);
    for(int i=0; i < num_args; i++) {
        tmp = va_arg(arg_list, char *);
        if(tmp) {
            total_length += str_length(tmp);
        }
    }
    va_end(arg_list);

    /* make a buffer big enough */
    total_length += 1;

    result = mem_alloc(total_length);
    if(!result) {
        pdebug(DEBUG_ERROR,"Unable to allocate new string buffer!");
        return NULL;
    }

    /* loop to copy the strings */
    result[0] = 0;
    va_start(arg_list, num_args);
    for(int i=0; i < num_args; i++) {
        tmp = va_arg(arg_list, char *);
        if(tmp) {
            int len = str_length(result);
            str_copy(&result[len], total_length - len, tmp);
        }
    }
    va_end(arg_list);

    return result;
}




/***************************************************************************
 ******************************* Mutexes ***********************************
 **************************************************************************/

struct mutex_t {
    HANDLE h_mutex;
    int initialized;
};


int mutex_create(mutex_p *m)
{
    *m = (struct mutex_t *)mem_alloc(sizeof(struct mutex_t));
    if(! *m) {
        /*pdebug("null mutex pointer.");*/
        return PLCTAG_ERR_NULL_PTR;
    }

    /* set up the mutex */
    (*m)->h_mutex = CreateMutex(
                        NULL,                   /* default security attributes  */
                        FALSE,                  /* initially not owned          */
                        NULL);                  /* unnamed mutex                */

    if(!(*m)->h_mutex) {
        mem_free(*m);
        *m = NULL;
        /*pdebug("Error initializing mutex.");*/
        return PLCTAG_ERR_MUTEX_INIT;
    }

    (*m)->initialized = 1;

    /*pdebug("Done.");*/

    return PLCTAG_STATUS_OK;
}



int mutex_lock(mutex_p m)
{
    DWORD dwWaitResult;

    //pdebug("Starting");

    if(!m) {
        /*pdebug("null mutex pointer.");*/
        return PLCTAG_ERR_NULL_PTR;
    }

    if(!m->initialized) {
        return PLCTAG_ERR_MUTEX_INIT;
    }

    dwWaitResult = ~WAIT_OBJECT_0;

    /* FIXME - This will potentially hang forever! */
    while(dwWaitResult != WAIT_OBJECT_0) {
        dwWaitResult = WaitForSingleObject(m->h_mutex,INFINITE);
    }

    return PLCTAG_STATUS_OK;
}



int mutex_try_lock(mutex_p m)
{
    DWORD dwWaitResult;

    //pdebug("Starting");

    if(!m) {
        /*pdebug("null mutex pointer.");*/
        return PLCTAG_ERR_NULL_PTR;
    }

    if(!m->initialized) {
        return PLCTAG_ERR_MUTEX_INIT;
    }

    dwWaitResult = WaitForSingleObject(m->h_mutex, 0);
    if(dwWaitResult == WAIT_OBJECT_0) {
        /* we got the lock */
        return PLCTAG_STATUS_OK;
    } else {
        return PLCTAG_ERR_MUTEX_LOCK;
    }
}



int mutex_unlock(mutex_p m)
{
    //pdebug("Starting.");

    if(!m) {
        /*pdebug("null mutex pointer.");*/
        return PLCTAG_ERR_NULL_PTR;
    }

    if(!m->initialized) {
        return PLCTAG_ERR_MUTEX_INIT;
    }

    if(!ReleaseMutex(m->h_mutex)) {
        /*pdebug("error unlocking mutex.");*/
        return PLCTAG_ERR_MUTEX_UNLOCK;
    }

    //pdebug("Done.");

    return PLCTAG_STATUS_OK;
}




int mutex_destroy(mutex_p *m)
{
    /*pdebug("Starting.");*/

    if(!m) {
        /*pdebug("null mutex pointer.");*/
        return PLCTAG_ERR_NULL_PTR;
    }

    CloseHandle((*m)->h_mutex);

    mem_free(*m);

    *m = NULL;

    /*pdebug("Done.");*/

    return PLCTAG_STATUS_OK;
}





/***************************************************************************
 ******************************* Threads ***********************************
 **************************************************************************/

struct thread_t {
    HANDLE h_thread;
    int initialized;
};


/*
 * thread_create()
 *
 * Start up a new thread.  This allocates the thread_t structure and starts
 * the passed function.  The arg argument is passed to the function.
 *
 * FIXME - use the stacksize!
 */

extern int thread_create(thread_p *t, LPTHREAD_START_ROUTINE func, int stacksize, void *arg)
{
    pdebug(DEBUG_DETAIL, "Starting.");

    if(!t) {
        pdebug(DEBUG_WARN, "null pointer to thread pointer!");
        return PLCTAG_ERR_NULL_PTR;
    }

    *t = (thread_p)mem_alloc(sizeof(struct thread_t));
    if(! *t) {
        /* FIXME - should not be the same error as above. */
        pdebug(DEBUG_WARN, "Unable to create new thread struct!");
        return PLCTAG_ERR_NO_MEM;
    }

    /* create a thread. */
    (*t)->h_thread = CreateThread(
                         NULL,                   /* default security attributes */
                         0,                      /* use default stack size      */
                         func,                   /* thread function             */
                         arg,                    /* argument to thread function */
                         0,                      /* use default creation flags  */
                         NULL);                  /* do not need thread ID       */

    if(!(*t)->h_thread) {
        pdebug(DEBUG_WARN, "error creating thread.");
        mem_free(*t);
        *t=NULL;

        return PLCTAG_ERR_THREAD_CREATE;
    }

    /* mark as initialized */
    (*t)->initialized = 1;

    pdebug(DEBUG_DETAIL, "Done.");

    return PLCTAG_STATUS_OK;
}




/*
 * thread_stop()
 *
 * Stop the current thread.  Does not take arguments.  Note: the thread
 * ends completely and this function does not return!
 */
void thread_stop(void)
{
    ExitThread((DWORD)0);
}


/*
 * thread_kill()
 *
 * Stop the indicated thread completely.
 */

void thread_kill(thread_p t)
{
    if(t) {
        TerminateThread(t->h_thread, (DWORD)0);
    }
}


/*
 * thread_join()
 *
 * Wait for the argument thread to stop and then continue.
 */

int thread_join(thread_p t)
{
    /*pdebug("Starting.");*/

    if(!t) {
        /*pdebug("null thread pointer.");*/
        return PLCTAG_ERR_NULL_PTR;
    }

    /* FIXME - check for uninitialized threads */

    if(WaitForSingleObject(t->h_thread, (DWORD)1000)) { /* FIXME - magic timeout */
        /*pdebug("Error joining thread.");*/
        return PLCTAG_ERR_THREAD_JOIN;
    }

    /*pdebug("Done.");*/

    return PLCTAG_STATUS_OK;
}




/*
 * thread_detach
 *
 * Detach the thread.  You cannot call thread_join on a detached thread!
 */

extern int thread_detach()
{
    /* FIXME - it does not look like you can do this on Windows??? */

    return PLCTAG_STATUS_OK;
}





/*
 * thread_destroy
 *
 * This gets rid of the resources of a thread struct.  The thread in
 * question must be dead first!
 */
extern int thread_destroy(thread_p *t)
{
    /*pdebug("Starting.");*/

    if(!t || ! *t) {
        /*pdebug("null thread pointer.");*/
        return PLCTAG_ERR_NULL_PTR;
    }

    CloseHandle((*t)->h_thread);

    mem_free(*t);

    *t = NULL;

    return PLCTAG_STATUS_OK;
}





/***************************************************************************
 ******************************* Atomic Ops ********************************
 **************************************************************************/

/*
 * lock_acquire
 *
 * Tries to write a non-zero value into the lock atomically.
 *
 * Returns non-zero on success.
 *
 * Warning: do not pass null pointers!
 */

#define ATOMIC_UNLOCK_VAL ((LONG)(0))
#define ATOMIC_LOCK_VAL ((LONG)(1))

extern int lock_acquire_try(lock_t *lock)
{
    LONG rc = InterlockedExchange(lock, ATOMIC_LOCK_VAL);

    if(rc != ATOMIC_LOCK_VAL) {
        return 1;
    } else {
        return 0;
    }
}


extern int lock_acquire(lock_t *lock)
{
    while(!lock_acquire_try(lock)) ;

    return 1;
}

extern void lock_release(lock_t *lock)
{
    InterlockedExchange(lock, ATOMIC_UNLOCK_VAL);
    /*pdebug("released lock");*/
}









/***************************************************************************
 ******************************** Sockets **********************************
 **************************************************************************/


struct sock_t {
    SOCKET fd;
    int port;
    int is_open;
};


#define MAX_IPS (8)


/* windows needs to have the Winsock library initialized
 * before use. Does it need to be static?
 */

static WSADATA wsaData;

static int socket_lib_init(void)
{
    return WSAStartup(MAKEWORD(2,2), &wsaData) == NO_ERROR;
}



extern int socket_create(sock_p *s)
{
    pdebug(DEBUG_DETAIL, "Starting.");

    if(!socket_lib_init()) {
        pdebug(DEBUG_WARN,"error initializing Windows Sockets.");
        return PLCTAG_ERR_WINSOCK;
    }

    if(!s) {
        pdebug(DEBUG_WARN, "null socket pointer.");
        return PLCTAG_ERR_NULL_PTR;
    }

    *s = (sock_p)mem_alloc(sizeof(struct sock_t));

    if(! *s) {
        pdebug(DEBUG_ERROR, "Unable to allocate memory for socket!");
        return PLCTAG_ERR_NO_MEM;
    }

    pdebug(DEBUG_DETAIL, "Done.");

    return PLCTAG_STATUS_OK;
}



extern int socket_connect_tcp(sock_p s, const char *host, int port)
{
    IN_ADDR ips[MAX_IPS];
    int num_ips = 0;
    struct sockaddr_in gw_addr;
    int sock_opt = 1;
    u_long non_blocking=1;
    int i = 0;
    int done = 0;
    SOCKET fd;
    struct timeval timeout; /* used for timing out connections etc. */
    struct linger so_linger;

    pdebug(DEBUG_DETAIL, "Starting.");

    /* Open a socket for communication with the gateway. */
    fd = socket(AF_INET, SOCK_STREAM, 0/*IPPROTO_TCP*/);

    /* check for errors */
    if(fd < 0) {
        /*pdebug("Socket creation failed, errno: %d",errno);*/
        return PLCTAG_ERR_OPEN;
    }

    /* set up our socket to allow reuse if we crash suddenly. */
    sock_opt = 1;

    if(setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,(char*)&sock_opt,sizeof(sock_opt))) {
        closesocket(fd);
        pdebug(DEBUG_WARN,"Error setting socket reuse option, errno: %d",errno);
        return PLCTAG_ERR_OPEN;
    }

    timeout.tv_sec = 10;
    timeout.tv_usec = 0;

    if(setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout))) {
        closesocket(fd);
        pdebug(DEBUG_WARN,"Error setting socket receive timeout option, errno: %d",errno);
        return PLCTAG_ERR_OPEN;
    }

    if(setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(timeout))) {
        closesocket(fd);
        pdebug(DEBUG_WARN,"Error setting socket set timeout option, errno: %d",errno);
        return PLCTAG_ERR_OPEN;
    }

    /* abort the connection on close. */
    so_linger.l_onoff = 1;
    so_linger.l_linger = 0;

    if(setsockopt(fd, SOL_SOCKET, SO_LINGER,(char*)&so_linger,sizeof(so_linger))) {
        closesocket(fd);
        pdebug(DEBUG_ERROR,"Error setting socket close linger option, errno: %d",errno);
        return PLCTAG_ERR_OPEN;
    }

    /* figure out what address we are connecting to. */

    /* try a numeric IP address conversion first. */
    if(inet_pton(AF_INET,host,(struct in_addr *)ips) > 0) {
        pdebug(DEBUG_DETAIL, "Found numeric IP address: %s", host);
        num_ips = 1;
    } else {
        struct addrinfo hints;
        struct addrinfo *res = NULL;
        int rc = 0;

        mem_set(&ips, 0, sizeof(ips));
        mem_set(&hints, 0, sizeof(hints));

        hints.ai_socktype = SOCK_STREAM; /* TCP */
        hints.ai_family = AF_INET; /* IP V4 only */

        if ((rc = getaddrinfo(host, NULL, &hints, &res)) != 0) {
            pdebug(DEBUG_WARN, "Error looking up PLC IP address %s, error = %d\n", host, rc);

            if (res) {
                freeaddrinfo(res);
            }

            return PLCTAG_ERR_BAD_GATEWAY;
        }

        for (num_ips = 0; res && num_ips < MAX_IPS; num_ips++) {
            ips[num_ips].s_addr = ((struct sockaddr_in *)(res->ai_addr))->sin_addr.s_addr;
            res = res->ai_next;
        }

        freeaddrinfo(res);
    }


    /* now try to connect to the remote gateway.  We may need to
     * try several of the IPs we have.
     */

    i = 0;
    done = 0;

    memset((void *)&gw_addr,0, sizeof(gw_addr));
    gw_addr.sin_family = AF_INET ;
    gw_addr.sin_port = htons(port);

    do {
        int rc;
        /* try each IP until we run out or get a connection. */
        gw_addr.sin_addr.s_addr = ips[i].s_addr;

        /*pdebug(DEBUG_DETAIL,"Attempting to connect to %s",inet_ntoa(*((struct in_addr *)&ips[i])));*/

        rc = connect(fd,(struct sockaddr *)&gw_addr,sizeof(gw_addr));

        if( rc == 0) {
            /* Windows MSVC does not like inet_ntoa(), not safe.
             * pdebug(DEBUG_DETAIL, "Attempt to connect to %s succeeded.",inet_ntoa(*((struct in_addr *)&ips[i])));
             */
            done = 1;
        } else {
            /* MSVC does not like inet_ntoa(), not safe.
             * pdebug(DEBUG_DETAIL, "Attempt to connect to %s failed, errno: %d",inet_ntoa(*((struct in_addr *)&ips[i])),errno);
             */
            i++;
        }
    } while(!done && i < num_ips);

    if(!done) {
        closesocket(fd);
        pdebug(DEBUG_WARN,"Unable to connect to any gateway host IP address!");
        return PLCTAG_ERR_OPEN;
    }


    /* FIXME
     * connect() is a little easier to handle in blocking mode, for now
     * we make the socket non-blocking here, after connect().
     */

    if(ioctlsocket(fd,FIONBIO,&non_blocking)) {
        /*pdebug("Error getting socket options, errno: %d", errno);*/
        closesocket(fd);
        return PLCTAG_ERR_OPEN;
    }

    /* save the values */
    s->fd = fd;
    s->port = port;
    s->is_open = 1;

    pdebug(DEBUG_DETAIL, "Done.");

    return PLCTAG_STATUS_OK;
}










extern int socket_read(sock_p s, uint8_t *buf, int size)
{
    int rc;

    if(!s || !buf) {
        return PLCTAG_ERR_NULL_PTR;
    }

    /* The socket is non-blocking. */
    rc = recv(s->fd, (char *)buf, size, 0);

    if(rc < 0) {
        int err = WSAGetLastError();

        if(err == WSAEWOULDBLOCK) {
            return 0;
        } else {
            pdebug(DEBUG_WARN,"socket read error rc=%d, errno=%d", rc, err);
            return PLCTAG_ERR_READ;
        }
    }

    return rc;
}


extern int socket_write(sock_p s, uint8_t *buf, int size)
{
    int rc;

    if(!s || !buf) {
        return PLCTAG_ERR_NULL_PTR;
    }

    /* The socket is non-blocking. */
    rc = send(s->fd, (char *)buf, size, MSG_NOSIGNAL);

    if(rc < 0) {
        int err = WSAGetLastError();

        if(err == WSAEWOULDBLOCK) {
            return PLCTAG_ERR_NO_DATA;
        } else {
            pdebug(DEBUG_WARN,"socket write error rc=%d, errno=%d", rc, err);
            return PLCTAG_ERR_WRITE;
        }
    }

    return rc;
}



extern int socket_close(sock_p s)
{
    if(!s)
        return PLCTAG_ERR_NULL_PTR;

    if(!s->is_open) {
        return PLCTAG_STATUS_OK;
    }

    if(!closesocket(s->fd)) {
        return PLCTAG_ERR_CLOSE;
    }

    s->fd = 0;
    s->is_open = 0;

    return PLCTAG_STATUS_OK;
}


extern int socket_destroy(sock_p *s)
{
    if(!s || !*s)
        return PLCTAG_ERR_NULL_PTR;

    socket_close(*s);

    mem_free(*s);

    *s = 0;

    if(WSACleanup() != NO_ERROR)
        return PLCTAG_ERR_WINSOCK;

    return PLCTAG_STATUS_OK;
}







/***************************************************************************
 ****************************** Serial Port ********************************
 **************************************************************************/


struct serial_port_t {
    HANDLE hSerialPort;
    COMMCONFIG oldDCBSerialParams;
    COMMTIMEOUTS oldTimeouts;
};


serial_port_p plc_lib_open_serial_port(/*plc_lib lib,*/ const char *path, int baud_rate, int data_bits, int stop_bits, int parity_type)
{
    serial_port_p serial_port;
    COMMCONFIG dcbSerialParams;
    COMMTIMEOUTS timeouts;
    HANDLE hSerialPort;
    int BAUD, PARITY, DATABITS, STOPBITS;

    //plc_err(lib, PLC_LOG_DEBUG, PLC_ERR_NONE, "Starting.");


    /* create the configuration for the serial port. */

    /* code largely from Programmer's Heaven.
     */

    switch (baud_rate) {
    case 38400:
        BAUD = CBR_38400;
        break;
    case 19200:
        BAUD  = CBR_19200;
        break;
    case 9600:
        BAUD  = CBR_9600;
        break;
    case 4800:
        BAUD  = CBR_4800;
        break;
    case 2400:
        BAUD  = CBR_2400;
        break;
    case 1200:
        BAUD  = CBR_1200;
        break;
    case 600:
        BAUD  = CBR_600;
        break;
    case 300:
        BAUD  = CBR_300;
        break;
    case 110:
        BAUD  = CBR_110;
        break;
    default:
        /* unsupported baud rate */
        //plc_err(lib, PLC_LOG_ERR, PLC_ERR_BAD_PARAM,"Unsupported baud rate: %d. Use standard baud rates (300,600,1200,2400...).",baud_rate);
        return NULL;
    }


    /* data bits */
    switch(data_bits) {
    case 5:
        DATABITS = 5;
        break;

    case 6:
        DATABITS = 6;
        break;

    case 7:
        DATABITS = 7;
        break;

    case 8:
        DATABITS = 8;
        break;

    default:
        /* unsupported number of data bits. */
        //plc_err(lib, PLC_LOG_ERR, PLC_ERR_BAD_PARAM,"Unsupported number of data bits: %d. Use 5-8.",data_bits);
        return NULL;
    }


    switch(stop_bits) {
    case 1:
        STOPBITS = ONESTOPBIT;
        break;
    case 2:
        STOPBITS = TWOSTOPBITS;
        break;
    default:
        /* unsupported number of stop bits. */
        //plc_err(lib, PLC_LOG_ERR, PLC_ERR_BAD_PARAM,"Unsupported number of stop bits, %d, must be 1 or 2.",stop_bits);
        return NULL;
    }

    switch(parity_type) {
    case 0:
        PARITY = NOPARITY;
        break;
    case 1: /* Odd parity */
        PARITY = ODDPARITY;
        break;
    case 2: /* Even parity */
        PARITY = EVENPARITY;
        break;
    default:
        /* unsupported number of stop bits. */
        //plc_err(lib, PLC_LOG_ERR, PLC_ERR_BAD_PARAM,"Unsupported parity type, must be none (0), odd (1) or even (2).");
        return NULL;
    }

    /* allocate the structure */
    serial_port = (serial_port_p)calloc(1,sizeof(struct serial_port_t));

    if(!serial_port) {
        //plc_err(lib, PLC_LOG_ERR, PLC_ERR_NO_MEM, "Unable to allocate serial port struct.");
        return NULL;
    }

    /* open the serial port device */
    hSerialPort = CreateFile(path,
                             GENERIC_READ | GENERIC_WRITE,
                             0,
                             NULL,
                             OPEN_EXISTING,
                             FILE_ATTRIBUTE_NORMAL,
                             NULL);

    /* did the open succeed? */
    if(hSerialPort == INVALID_HANDLE_VALUE) {
        free(serial_port);
        //plc_err(lib, PLC_LOG_ERR, PLC_ERR_OPEN, "Error opening serial device %s",path);
        return NULL;
    }

    /* get existing serial port configuration and save it. */
    if(!GetCommState(hSerialPort, &(serial_port->oldDCBSerialParams.dcb))) {
        free(serial_port);
        CloseHandle(hSerialPort);
        //plc_err(lib, PLC_LOG_ERR, PLC_ERR_OPEN, "Error getting backup serial port configuration.",path);
        return NULL;
    }

    /* copy the params. */
    dcbSerialParams = serial_port->oldDCBSerialParams;

    dcbSerialParams.dcb.BaudRate = BAUD;
    dcbSerialParams.dcb.ByteSize = DATABITS;
    dcbSerialParams.dcb.StopBits = STOPBITS;
    dcbSerialParams.dcb.Parity = PARITY;

    dcbSerialParams.dcb.fBinary         = TRUE;
    dcbSerialParams.dcb.fDtrControl     = DTR_CONTROL_DISABLE;
    dcbSerialParams.dcb.fRtsControl     = RTS_CONTROL_DISABLE;
    dcbSerialParams.dcb.fOutxCtsFlow    = FALSE;
    dcbSerialParams.dcb.fOutxDsrFlow    = FALSE;
    dcbSerialParams.dcb.fDsrSensitivity = FALSE;
    dcbSerialParams.dcb.fAbortOnError   = TRUE; /* FIXME - should this be false? */

    /* attempt to set the serial params */
    if(!SetCommState(hSerialPort, &dcbSerialParams.dcb)) {
        free(serial_port);
        CloseHandle(hSerialPort);
        //plc_err(lib, PLC_LOG_ERR, PLC_ERR_OPEN, "Error setting serial port configuration.",path);
        return NULL;
    }

    /* attempt to get the current serial port timeout set up */
    if(!GetCommTimeouts(hSerialPort, &(serial_port->oldTimeouts))) {
        SetCommState(hSerialPort, &(serial_port->oldDCBSerialParams.dcb));
        free(serial_port);
        CloseHandle(hSerialPort);
        //plc_err(lib, PLC_LOG_ERR, PLC_ERR_OPEN, "Error getting backup serial port timeouts.",path);
        return NULL;
    }

    timeouts = serial_port->oldTimeouts;

    /* set the timeouts for asynch operation */
    timeouts.ReadIntervalTimeout = MAXDWORD;
    timeouts.ReadTotalTimeoutMultiplier = 0;
    timeouts.ReadTotalTimeoutConstant = 0;
    timeouts.WriteTotalTimeoutMultiplier = 0;
    timeouts.WriteTotalTimeoutConstant = 0;

    /* attempt to set the current serial port timeout set up */
    if(!SetCommTimeouts(hSerialPort, &timeouts)) {
        SetCommState(hSerialPort, &(serial_port->oldDCBSerialParams.dcb));
        free(serial_port);
        CloseHandle(hSerialPort);
        //plc_err(lib, PLC_LOG_ERR, PLC_ERR_OPEN, "Error getting backup serial port timeouts.",path);
        return NULL;
    }

    return serial_port;
}







int plc_lib_close_serial_port(serial_port_p serial_port)
{
    /* try to prevent this from being called twice */
    if(!serial_port || !serial_port->hSerialPort) {
        return 1;
    }

    /* reset the old options */
    SetCommTimeouts(serial_port->hSerialPort, &(serial_port->oldTimeouts));
    SetCommState(serial_port->hSerialPort, &(serial_port->oldDCBSerialParams.dcb));
    CloseHandle(serial_port->hSerialPort);

    /* make sure that we do not call this twice. */
    serial_port->hSerialPort = 0;

    /* free the serial port */
    free(serial_port);

    return 1;
}




int plc_lib_serial_port_read(serial_port_p serial_port, uint8_t *data, int size)
{
    DWORD numBytesRead = 0;
    BOOL rc;

    rc = ReadFile(serial_port->hSerialPort,(LPVOID)data,(DWORD)size,&numBytesRead,NULL);

    if(rc != TRUE)
        return -1;

    return (int)numBytesRead;
}


int plc_lib_serial_port_write(serial_port_p serial_port, uint8_t *data, int size)
{
    DWORD numBytesWritten = 0;
    BOOL rc;

    rc = WriteFile(serial_port->hSerialPort,(LPVOID)data,(DWORD)size,&numBytesWritten,NULL);

    return (int)numBytesWritten;
}











/***************************************************************************
 ***************************** Miscellaneous *******************************
 **************************************************************************/






int sleep_ms(int ms)
{
    Sleep(ms);
    return 1;
}



/*
 * time_ms
 *
 * Return current system time in millisecond units.  This is NOT an
 * Unix epoch time.  Windows uses a different epoch starting 1/1/1601.
 */

int64_t time_ms(void)
{
    FILETIME ft;
    int64_t res;

    GetSystemTimeAsFileTime(&ft);

    /* calculate time as 100ns increments since Jan 1, 1601. */
    res = (int64_t)(ft.dwLowDateTime) + ((int64_t)(ft.dwHighDateTime) << 32);

    /* get time in ms */

    res = res / 10000;

    return  res;
}


struct tm *localtime_r(const time_t *timep, struct tm *result)
{
    time_t t = *timep;

    localtime_s(result, &t);

    return result;
}




/*#ifdef __cplusplus
}
#endif
*/
