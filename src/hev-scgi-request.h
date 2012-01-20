/*
 ============================================================================
 Name        : hev-scgi-request.h
 Author      : Heiher <admin@heiher.info>
 Version     : 0.0.1
 Copyright   : Copyright (C) 2011 everyone.
 Description : 
 ============================================================================
 */

#ifndef __HEV_SCGI_REQUEST_H__
#define __HEV_SCGI_REQUEST_H__

#include <glib-object.h>
#include <gio/gio.h>

G_BEGIN_DECLS

#define HEV_TYPE_SCGI_REQUEST	(hev_scgi_request_get_type())
#define HEV_SCGI_REQUEST(obj)	(G_TYPE_CHECK_INSTANCE_CAST((obj), HEV_TYPE_SCGI_REQUEST, HevSCGIRequest))
#define HEV_IS_SCGI_REQUEST(obj)	(G_TYPE_CHECK_INSTANCE_TYPE((obj), HEV_TYPE_SCGI_REQUEST))
#define HEV_SCGI_REQUEST_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST((klass), HEV_TYPE_SCGI_REQUEST, HevSCGIRequestClass))
#define HEV_IS_SCGI_REQUEST_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE((klass), HEV_TYPE_SCGI_REQUEST))
#define HEV_SCGI_REQUEST_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS((obj), HEV_TYPE_SCGI_REQUEST, HevSCGIRequestClass))

typedef struct _HevSCGIRequest HevSCGIRequest;
typedef struct _HevSCGIRequestClass HevSCGIRequestClass;

struct _HevSCGIRequest
{
	GObject parent_instance;
};

struct _HevSCGIRequestClass
{
	GObjectClass parent_class;
};

GType hev_scgi_request_get_type(void);

GObject * hev_scgi_request_new(void);
void hev_scgi_request_set_input_stream(HevSCGIRequest *self,
			GInputStream *input_stream);
GInputStream * hev_scgi_request_get_input_stream(HevSCGIRequest *self);
void hev_scgi_request_read_header(HevSCGIRequest *self,
			GFunc callback, gpointer user_data);
GHashTable * hev_scgi_request_get_header_hash_table(HevSCGIRequest *self);

G_END_DECLS

#endif /* __HEV_SCGI_REQUEST_H__ */

