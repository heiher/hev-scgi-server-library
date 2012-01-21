/*
 ============================================================================
 Name        : hev-scgi-handler-module.h
 Author      : Heiher <admin@heiher.info>
 Version     : 0.0.1
 Copyright   : Copyright (C) 2011 everyone.
 Description : 
 ============================================================================
 */

#ifndef __HEV_SCGI_HANDLER_MODULE_H__
#define __HEV_SCGI_HANDLER_MODULE_H__

#include <glib-object.h>

G_BEGIN_DECLS

#define HEV_TYPE_SCGI_HANDLER_MODULE	(hev_scgi_handler_module_get_type())
#define HEV_SCGI_HANDLER_MODULE(obj)	(G_TYPE_CHECK_INSTANCE_CAST((obj), HEV_TYPE_SCGI_HANDLER_MODULE, HevSCGIHandlerModule))
#define HEV_IS_SCGI_HANDLER_MODULE(obj)	(G_TYPE_CHECK_INSTANCE_TYPE((obj), HEV_TYPE_SCGI_HANDLER_MODULE))
#define HEV_SCGI_HANDLER_MODULE_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST((klass), HEV_TYPE_SCGI_HANDLER_MODULE, HevSCGIHandlerModuleClass))
#define HEV_IS_SCGI_HANDLER_MODULE_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE((klass), HEV_TYPE_SCGI_HANDLER_MODULE))
#define HEV_SCGI_HANDLER_MODULE_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS((obj), HEV_TYPE_SCGI_HANDLER_MODULE, HevSCGIHandlerModuleClass))

typedef struct _HevSCGIHandlerModule HevSCGIHandlerModule;
typedef struct _HevSCGIHandlerModuleClass HevSCGIHandlerModuleClass;

struct _HevSCGIHandlerModule
{
	GTypeModule parent_instance;
};

struct _HevSCGIHandlerModuleClass
{
	GTypeModuleClass parent_class;
};

GType hev_scgi_handler_module_get_type(void);

GObject * hev_scgi_handler_module_new(const gchar *path);

G_END_DECLS

#endif /* __HEV_SCGI_HANDLER_MODULE_H__ */

