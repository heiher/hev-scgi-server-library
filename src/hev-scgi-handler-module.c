/*
 ============================================================================
 Name        : hev-scgi-handler-module.c
 Author      : Heiher <admin@heiher.info>
 Version     : 0.0.1
 Copyright   : Copyright (C) 2011 everyone.
 Description : 
 ============================================================================
 */

#include <gmodule.h>

#include "hev-scgi-handler-module.h"
#include "hev-scgi-handler.h"

enum
{
	PROP_0,
	PROP_PATH,
	N_PROPERTIES
};

static GParamSpec *hev_scgi_handler_module_properties[N_PROPERTIES] = { NULL };

#define HEV_SCGI_HANDLER_MODULE_GET_PRIVATE(obj)	(G_TYPE_INSTANCE_GET_PRIVATE((obj), HEV_TYPE_SCGI_HANDLER_MODULE, HevSCGIHandlerModulePrivate))

typedef struct _HevSCGIHandlerModulePrivate HevSCGIHandlerModulePrivate;

struct _HevSCGIHandlerModulePrivate
{
	gchar *path;
	GModule *module;
};

static void hev_scgi_handler_iface_init(HevSCGIHandlerInterface * iface);

G_DEFINE_TYPE_WITH_CODE(HevSCGIHandlerModule, hev_scgi_handler_module, G_TYPE_TYPE_MODULE,
			G_IMPLEMENT_INTERFACE(HEV_TYPE_SCGI_HANDLER, hev_scgi_handler_iface_init));

static void hev_scgi_handler_module_dispose(GObject * obj)
{
	HevSCGIHandlerModule * self = HEV_SCGI_HANDLER_MODULE(obj);
	HevSCGIHandlerModulePrivate * priv = HEV_SCGI_HANDLER_MODULE_GET_PRIVATE(self);

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	G_OBJECT_CLASS(hev_scgi_handler_module_parent_class)->dispose(obj);
}

static void hev_scgi_handler_module_finalize(GObject * obj)
{
	HevSCGIHandlerModule * self = HEV_SCGI_HANDLER_MODULE(obj);
	HevSCGIHandlerModulePrivate * priv = HEV_SCGI_HANDLER_MODULE_GET_PRIVATE(self);

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	g_type_module_unuse(G_TYPE_MODULE(obj));

	if(priv->path)
	{
		g_free(priv->path);
		priv->path = NULL;
	}

	G_OBJECT_CLASS(hev_scgi_handler_module_parent_class)->finalize(obj);
}

static GObject * hev_scgi_handler_module_constructor(GType type, guint n, GObjectConstructParam * param)
{
	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);
	return G_OBJECT_CLASS(hev_scgi_handler_module_parent_class)->constructor(type, n, param);
}

static void hev_scgi_handler_module_constructed(GObject * obj)
{
	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	g_type_module_use(G_TYPE_MODULE(obj));
}

static gboolean hev_scgi_handler_module_load(GTypeModule *obj)
{
	HevSCGIHandlerModule * self = HEV_SCGI_HANDLER_MODULE(obj);
	HevSCGIHandlerModulePrivate * priv = HEV_SCGI_HANDLER_MODULE_GET_PRIVATE(self);
	HevSCGIHandlerInterface *iface = HEV_SCGI_HANDLER_GET_INTERFACE(obj);

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	priv->module = g_module_open(priv->path, G_MODULE_BIND_LAZY);
	if(!priv->module)
	{
		g_critical("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);
		return FALSE;
	}

	if(!g_module_symbol(priv->module, "hev_scgi_handler_module_get_name",
					(gpointer)&iface->get_name))
	{
		g_critical("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);
		g_module_close(priv->module);
		return FALSE;
	}

	if(!g_module_symbol(priv->module, "hev_scgi_handler_module_get_version",
					(gpointer)&iface->get_version))
	{
		g_critical("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);
		g_module_close(priv->module);
		return FALSE;
	}

	if(!g_module_symbol(priv->module, "hev_scgi_handler_module_get_pattern",
					(gpointer)&iface->get_pattern))
	{
		g_critical("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);
		g_module_close(priv->module);
		return FALSE;
	}

	if(!g_module_symbol(priv->module, "hev_scgi_handler_module_handle",
					(gpointer)&iface->handle))
	{
		g_critical("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);
		g_module_close(priv->module);
		return FALSE;
	}

	return TRUE;
}

static void hev_scgi_handler_module_unload(GTypeModule *obj)
{
	HevSCGIHandlerModule * self = HEV_SCGI_HANDLER_MODULE(obj);
	HevSCGIHandlerModulePrivate * priv = HEV_SCGI_HANDLER_MODULE_GET_PRIVATE(self);
	HevSCGIHandlerInterface *iface = HEV_SCGI_HANDLER_GET_INTERFACE(obj);

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	iface->get_name = NULL;
	iface->get_version = NULL;
	iface->get_pattern = NULL;
	iface->handle = NULL;

	g_module_close(priv->module);
}

static void hev_scgi_handler_module_set_property(GObject *obj,
			guint prop_id, const GValue *value, GParamSpec *pspec)
{
	HevSCGIHandlerModule * self = HEV_SCGI_HANDLER_MODULE(obj);
	HevSCGIHandlerModulePrivate * priv = HEV_SCGI_HANDLER_MODULE_GET_PRIVATE(self);

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	switch(prop_id)
	{
	case PROP_PATH:
		priv->path = g_value_dup_string(value);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, prop_id, pspec);
		break;
	}
}

static void hev_scgi_handler_module_get_property(GObject *obj,
			guint prop_id, GValue *value, GParamSpec *pspec)
{
	HevSCGIHandlerModule * self = HEV_SCGI_HANDLER_MODULE(obj);
	HevSCGIHandlerModulePrivate * priv = HEV_SCGI_HANDLER_MODULE_GET_PRIVATE(self);

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	switch(prop_id)
	{
	case PROP_PATH: 
		g_value_set_string(value, priv->path);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, prop_id, pspec);
		break;
	}
}

static void hev_scgi_handler_module_class_init(HevSCGIHandlerModuleClass * klass)
{
	GObjectClass * obj_class = G_OBJECT_CLASS(klass);
	GTypeModuleClass *type_module_class = G_TYPE_MODULE_CLASS(klass);

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	obj_class->constructor = hev_scgi_handler_module_constructor;
	obj_class->constructed = hev_scgi_handler_module_constructed;
	obj_class->dispose = hev_scgi_handler_module_dispose;
	obj_class->finalize = hev_scgi_handler_module_finalize;
	obj_class->set_property = hev_scgi_handler_module_set_property;
	obj_class->get_property = hev_scgi_handler_module_get_property;

	type_module_class->load = hev_scgi_handler_module_load;
	type_module_class->unload = hev_scgi_handler_module_unload;

	hev_scgi_handler_module_properties[PROP_PATH] =
		g_param_spec_string ("path",
					"Path",
					"The path to use when loading this module",
					NULL,
					G_PARAM_READWRITE |	G_PARAM_CONSTRUCT_ONLY);
	g_object_class_install_properties(obj_class, N_PROPERTIES,
				hev_scgi_handler_module_properties);

	g_type_class_add_private(klass, sizeof(HevSCGIHandlerModulePrivate));
}

static void hev_scgi_handler_module_class_finalize(HevSCGIHandlerModuleClass * klass)
{
	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);
}

static void hev_scgi_handler_module_init(HevSCGIHandlerModule * self)
{
	HevSCGIHandlerModulePrivate * priv = HEV_SCGI_HANDLER_MODULE_GET_PRIVATE(self);

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	priv->path = NULL;
}

static void hev_scgi_handler_iface_init(HevSCGIHandlerInterface * iface)
{
	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);
}


GObject * hev_scgi_handler_module_new(const gchar *path)
{
	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);
	return g_object_new(HEV_TYPE_SCGI_HANDLER_MODULE,
				"path", path, NULL);
}

