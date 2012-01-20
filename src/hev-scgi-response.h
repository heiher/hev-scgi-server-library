/*
 ============================================================================
 Name        : hev-scgi-response.h
 Author      : Heiher <admin@heiher.info>
 Version     : 0.0.1
 Copyright   : Copyright (C) 2011 everyone.
 Description : 
 ============================================================================
 */

#ifndef __HEV_SCGI_RESPONSE_H__
#define __HEV_SCGI_RESPONSE_H__

#include <glib-object.h>
#include <gio/gio.h>

G_BEGIN_DECLS

#define HEV_TYPE_SCGI_RESPONSE	(hev_scgi_response_get_type())
#define HEV_SCGI_RESPONSE(obj)	(G_TYPE_CHECK_INSTANCE_CAST((obj), HEV_TYPE_SCGI_RESPONSE, HevSCGIResponse))
#define HEV_IS_SCGI_RESPONSE(obj)	(G_TYPE_CHECK_INSTANCE_TYPE((obj), HEV_TYPE_SCGI_RESPONSE))
#define HEV_SCGI_RESPONSE_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST((klass), HEV_TYPE_SCGI_RESPONSE, HevSCGIResponseClass))
#define HEV_IS_SCGI_RESPONSE_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE((klass), HEV_TYPE_SCGI_RESPONSE))
#define HEV_SCGI_RESPONSE_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS((obj), HEV_TYPE_SCGI_RESPONSE, HevSCGIResponseClass))

typedef struct _HevSCGIResponse HevSCGIResponse;
typedef struct _HevSCGIResponseClass HevSCGIResponseClass;

struct _HevSCGIResponse
{
	GObject parent_instance;
};

struct _HevSCGIResponseClass
{
	GObjectClass parent_class;
};

GType hev_scgi_response_get_type(void);

GObject * hev_scgi_response_new(void);
void hev_scgi_response_set_output_stream(HevSCGIResponse *self,
			GOutputStream *output_stream);
GOutputStream * hev_scgi_response_get_output_stream(HevSCGIResponse *self);
void hev_scgi_response_write_header(HevSCGIResponse *self,
			GFunc callback, gpointer user_data);
GHashTable * hev_scgi_response_get_header_hash_table(HevSCGIResponse *self);

G_END_DECLS

#endif /* __HEV_SCGI_RESPONSE_H__ */

