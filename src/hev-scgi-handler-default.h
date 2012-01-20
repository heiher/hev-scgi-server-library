/*
 ============================================================================
 Name        : hev-scgi-handler-default.h
 Author      : Heiher <admin@heiher.info>
 Version     : 0.0.1
 Copyright   : Copyright (C) 2011 everyone.
 Description : 
 ============================================================================
 */

#ifndef __HEV_SCGI_HANDLER_DEFAULT_H__
#define __HEV_SCGI_HANDLER_DEFAULT_H__

#include <glib-object.h>

G_BEGIN_DECLS

#define HEV_TYPE_SCGI_HANDLER_DEFAULT	(hev_scgi_handler_default_get_type())
#define HEV_SCGI_HANDLER_DEFAULT(obj)	(G_TYPE_CHECK_INSTANCE_CAST((obj), HEV_TYPE_SCGI_HANDLER_DEFAULT, HevSCGIHandlerDefault))
#define HEV_IS_SCGI_HANDLER_DEFAULT(obj)	(G_TYPE_CHECK_INSTANCE_TYPE((obj), HEV_TYPE_SCGI_HANDLER_DEFAULT))
#define HEV_SCGI_HANDLER_DEFAULT_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST((klass), HEV_TYPE_SCGI_HANDLER_DEFAULT, HevSCGIHandlerDefaultClass))
#define HEV_IS_SCGI_HANDLER_DEFAULT_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE((klass), HEV_TYPE_SCGI_HANDLER_DEFAULT))
#define HEV_SCGI_HANDLER_DEFAULT_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS((obj), HEV_TYPE_SCGI_HANDLER_DEFAULT, HevSCGIHandlerDefaultClass))

typedef struct _HevSCGIHandlerDefault HevSCGIHandlerDefault;
typedef struct _HevSCGIHandlerDefaultClass HevSCGIHandlerDefaultClass;

struct _HevSCGIHandlerDefault
{
	GObject parent_instance;
};

struct _HevSCGIHandlerDefaultClass
{
	GObjectClass parent_class;
};

GType hev_scgi_handler_default_get_type(void);

GObject * hev_scgi_handler_default_new(void);

G_END_DECLS

#endif /* __HEV_SCGI_HANDLER_DEFAULT_H__ */

