/*
 ============================================================================
 Name        : hev-scgi_config.h
 Author      : Heiher <admin@heiher.info>
 Version     : 0.0.1
 Copyright   : Copyright (C) 2011 everyone.
 Description : 
 ============================================================================
 */

#ifndef __HEV_SCGI_CONFIG_H__
#define __HEV_SCGI_CONFIG_H__

#include <glib-object.h>
#include <gio/gio.h>

G_BEGIN_DECLS

#define HEV_TYPE_SCGI_CONFIG	(hev_scgi_config_get_type())
#define HEV_SCGI_CONFIG(obj)	(G_TYPE_CHECK_INSTANCE_CAST((obj), HEV_TYPE_SCGI_CONFIG, HevSCGIConfig))
#define HEV_IS_SCGI_CONFIG(obj)	(G_TYPE_CHECK_INSTANCE_TYPE((obj), HEV_TYPE_SCGI_CONFIG))
#define HEV_SCGI_CONFIG_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST((klass), HEV_TYPE_SCGI_CONFIG, HevSCGIConfigClass))
#define HEV_IS_SCGI_CONFIG_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE((klass), HEV_TYPE_SCGI_CONFIG))
#define HEV_SCGI_CONFIG_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS((obj), HEV_TYPE_SCGI_CONFIG, HevSCGIConfigClass))

typedef struct _HevSCGIConfig HevSCGIConfig;
typedef struct _HevSCGIConfigClass HevSCGIConfigClass;

struct _HevSCGIConfig
{
	GObject parent_instance;
};

struct _HevSCGIConfigClass
{
	GObjectClass parent_class;
};

GType hev_scgi_config_get_type(void);

GObject * hev_scgi_config_new(void);

GSocketAddress * hev_scgi_config_get_address(HevSCGIConfig *self);

G_END_DECLS

#endif /* __HEV_SCGI_CONFIG_H__ */

