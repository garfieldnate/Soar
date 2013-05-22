/** THIS IS AN AUTOMATICALLY GENERATED FILE.  DO NOT MODIFY
 * BY HAND!!
 *
 * Generated by lcm-gen
 **/

#include <string.h>
#include "pose_t.h"

static int __pose_t_hash_computed;
static int64_t __pose_t_hash;
 
int64_t __pose_t_hash_recursive(const __lcm_hash_ptr *p)
{
    const __lcm_hash_ptr *fp;
    for (fp = p; fp != NULL; fp = fp->parent)
        if (fp->v == __pose_t_get_hash)
            return 0;
 
    const __lcm_hash_ptr cp = { p, (void*)__pose_t_get_hash };
    (void) cp;
 
    int64_t hash = 0x170b77d82958082fLL
         + __int64_t_hash_recursive(&cp)
         + __double_hash_recursive(&cp)
         + __double_hash_recursive(&cp)
         + __double_hash_recursive(&cp)
         + __double_hash_recursive(&cp)
         + __double_hash_recursive(&cp)
        ;
 
    return (hash<<1) + ((hash>>63)&1);
}
 
int64_t __pose_t_get_hash(void)
{
    if (!__pose_t_hash_computed) {
        __pose_t_hash = __pose_t_hash_recursive(NULL);
        __pose_t_hash_computed = 1;
    }
 
    return __pose_t_hash;
}
 
int __pose_t_encode_array(void *buf, int offset, int maxlen, const pose_t *p, int elements)
{
    int pos = 0, thislen, element;
 
    for (element = 0; element < elements; element++) {
 
        thislen = __int64_t_encode_array(buf, offset + pos, maxlen - pos, &(p[element].utime), 1);
        if (thislen < 0) return thislen; else pos += thislen;
 
        thislen = __double_encode_array(buf, offset + pos, maxlen - pos, p[element].pos, 3);
        if (thislen < 0) return thislen; else pos += thislen;
 
        thislen = __double_encode_array(buf, offset + pos, maxlen - pos, p[element].vel, 3);
        if (thislen < 0) return thislen; else pos += thislen;
 
        thislen = __double_encode_array(buf, offset + pos, maxlen - pos, p[element].orientation, 4);
        if (thislen < 0) return thislen; else pos += thislen;
 
        thislen = __double_encode_array(buf, offset + pos, maxlen - pos, p[element].rotation_rate, 3);
        if (thislen < 0) return thislen; else pos += thislen;
 
        thislen = __double_encode_array(buf, offset + pos, maxlen - pos, p[element].accel, 3);
        if (thislen < 0) return thislen; else pos += thislen;
 
    }
    return pos;
}
 
int pose_t_encode(void *buf, int offset, int maxlen, const pose_t *p)
{
    int pos = 0, thislen;
    int64_t hash = __pose_t_get_hash();
 
    thislen = __int64_t_encode_array(buf, offset + pos, maxlen - pos, &hash, 1);
    if (thislen < 0) return thislen; else pos += thislen;
 
    thislen = __pose_t_encode_array(buf, offset + pos, maxlen - pos, p, 1);
    if (thislen < 0) return thislen; else pos += thislen;
 
    return pos;
}
 
int __pose_t_encoded_array_size(const pose_t *p, int elements)
{
    int size = 0, element;
    for (element = 0; element < elements; element++) {
 
        size += __int64_t_encoded_array_size(&(p[element].utime), 1);
 
        size += __double_encoded_array_size(p[element].pos, 3);
 
        size += __double_encoded_array_size(p[element].vel, 3);
 
        size += __double_encoded_array_size(p[element].orientation, 4);
 
        size += __double_encoded_array_size(p[element].rotation_rate, 3);
 
        size += __double_encoded_array_size(p[element].accel, 3);
 
    }
    return size;
}
 
int pose_t_encoded_size(const pose_t *p)
{
    return 8 + __pose_t_encoded_array_size(p, 1);
}
 
int __pose_t_decode_array(const void *buf, int offset, int maxlen, pose_t *p, int elements)
{
    int pos = 0, thislen, element;
 
    for (element = 0; element < elements; element++) {
 
        thislen = __int64_t_decode_array(buf, offset + pos, maxlen - pos, &(p[element].utime), 1);
        if (thislen < 0) return thislen; else pos += thislen;
 
        thislen = __double_decode_array(buf, offset + pos, maxlen - pos, p[element].pos, 3);
        if (thislen < 0) return thislen; else pos += thislen;
 
        thislen = __double_decode_array(buf, offset + pos, maxlen - pos, p[element].vel, 3);
        if (thislen < 0) return thislen; else pos += thislen;
 
        thislen = __double_decode_array(buf, offset + pos, maxlen - pos, p[element].orientation, 4);
        if (thislen < 0) return thislen; else pos += thislen;
 
        thislen = __double_decode_array(buf, offset + pos, maxlen - pos, p[element].rotation_rate, 3);
        if (thislen < 0) return thislen; else pos += thislen;
 
        thislen = __double_decode_array(buf, offset + pos, maxlen - pos, p[element].accel, 3);
        if (thislen < 0) return thislen; else pos += thislen;
 
    }
    return pos;
}
 
int __pose_t_decode_array_cleanup(pose_t *p, int elements)
{
    int element;
    for (element = 0; element < elements; element++) {
 
        __int64_t_decode_array_cleanup(&(p[element].utime), 1);
 
        __double_decode_array_cleanup(p[element].pos, 3);
 
        __double_decode_array_cleanup(p[element].vel, 3);
 
        __double_decode_array_cleanup(p[element].orientation, 4);
 
        __double_decode_array_cleanup(p[element].rotation_rate, 3);
 
        __double_decode_array_cleanup(p[element].accel, 3);
 
    }
    return 0;
}
 
int pose_t_decode(const void *buf, int offset, int maxlen, pose_t *p)
{
    int pos = 0, thislen;
    int64_t hash = __pose_t_get_hash();
 
    int64_t this_hash;
    thislen = __int64_t_decode_array(buf, offset + pos, maxlen - pos, &this_hash, 1);
    if (thislen < 0) return thislen; else pos += thislen;
    if (this_hash != hash) return -1;
 
    thislen = __pose_t_decode_array(buf, offset + pos, maxlen - pos, p, 1);
    if (thislen < 0) return thislen; else pos += thislen;
 
    return pos;
}
 
int pose_t_decode_cleanup(pose_t *p)
{
    return __pose_t_decode_array_cleanup(p, 1);
}
 
int __pose_t_clone_array(const pose_t *p, pose_t *q, int elements)
{
    int element;
    for (element = 0; element < elements; element++) {
 
        __int64_t_clone_array(&(p[element].utime), &(q[element].utime), 1);
 
        __double_clone_array(p[element].pos, q[element].pos, 3);
 
        __double_clone_array(p[element].vel, q[element].vel, 3);
 
        __double_clone_array(p[element].orientation, q[element].orientation, 4);
 
        __double_clone_array(p[element].rotation_rate, q[element].rotation_rate, 3);
 
        __double_clone_array(p[element].accel, q[element].accel, 3);
 
    }
    return 0;
}
 
pose_t *pose_t_copy(const pose_t *p)
{
    pose_t *q = (pose_t*) malloc(sizeof(pose_t));
    __pose_t_clone_array(p, q, 1);
    return q;
}
 
void pose_t_destroy(pose_t *p)
{
    __pose_t_decode_array_cleanup(p, 1);
    free(p);
}
 
int pose_t_publish(lcm_t *lc, const char *channel, const pose_t *p)
{
      int max_data_size = pose_t_encoded_size (p);
      uint8_t *buf = (uint8_t*) malloc (max_data_size);
      if (!buf) return -1;
      int data_size = pose_t_encode (buf, 0, max_data_size, p);
      if (data_size < 0) {
          free (buf);
          return data_size;
      }
      int status = lcm_publish (lc, channel, buf, data_size);
      free (buf);
      return status;
}

struct _pose_t_subscription_t {
    pose_t_handler_t user_handler;
    void *userdata;
    lcm_subscription_t *lc_h;
};
static
void pose_t_handler_stub (const lcm_recv_buf_t *rbuf, 
                            const char *channel, void *userdata)
{
    int status;
    pose_t p;
    memset(&p, 0, sizeof(pose_t));
    status = pose_t_decode (rbuf->data, 0, rbuf->data_size, &p);
    if (status < 0) {
        fprintf (stderr, "error %d decoding pose_t!!!\n", status);
        return;
    }

    pose_t_subscription_t *h = (pose_t_subscription_t*) userdata;
    h->user_handler (rbuf, channel, &p, h->userdata);

    pose_t_decode_cleanup (&p);
}

pose_t_subscription_t* pose_t_subscribe (lcm_t *lcm, 
                    const char *channel, 
                    pose_t_handler_t f, void *userdata)
{
    pose_t_subscription_t *n = (pose_t_subscription_t*)
                       malloc(sizeof(pose_t_subscription_t));
    n->user_handler = f;
    n->userdata = userdata;
    n->lc_h = lcm_subscribe (lcm, channel, 
                                 pose_t_handler_stub, n);
    if (n->lc_h == NULL) {
        fprintf (stderr,"couldn't reg pose_t LCM handler!\n");
        free (n);
        return NULL;
    }
    return n;
}

int pose_t_subscription_set_queue_capacity (pose_t_subscription_t* subs, 
                              int num_messages)
{
    return lcm_subscription_set_queue_capacity (subs->lc_h, num_messages);
}

int pose_t_unsubscribe(lcm_t *lcm, pose_t_subscription_t* hid)
{
    int status = lcm_unsubscribe (lcm, hid->lc_h);
    if (0 != status) {
        fprintf(stderr, 
           "couldn't unsubscribe pose_t_handler %p!\n", hid);
        return -1;
    }
    free (hid);
    return 0;
}
