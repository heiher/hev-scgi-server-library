/*
 ============================================================================
 Name        : hev-scgi-config.c
 Author      : Heiher <admin@heiher.info>
 Version     : 0.0.1
 Copyright   : Copyright (C) 2011 everyone.
 Description : 
 ============================================================================
 */

#include "hev-scgi-config.h"

#define HEV_SCGI_CONFIG_FILE_NAME			"main.conf"

#define HEV_SCGI_CONFIG_GET_PRIVATE(obj)	(G_TYPE_INSTANCE_GET_PRIVATE((obj), HEV_TYPE_SCGI_CONFIG, HevSCGIConfigPrivate))

typedef struct _HevSCGIConfigPrivate HevSCGIConfigPrivate;

struct _HevSCGIConfigPrivate
{
	GKeyFile *key_file;
};

G_DEFINE_TYPE(HevSCGIConfig, hev_scgi_config, G_TYPE_OBJECT);

static void hev_scgi_config_dispose(GObject * obj)
{
	HevSCGIConfig * self = HEV_SCGI_CONFIG(obj);
	HevSCGIConfigPrivate * priv = HEV_SCGI_CONFIG_GET_PRIVATE(self);

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	G_OBJECT_CLASS(hev_scgi_config_parent_class)->dispose(obj);
}

static void hev_scgi_config_finalize(GObject * obj)
{
	HevSCGIConfig * self = HEV_SCGI_CONFIG(obj);
	HevSCGIConfigPrivate * priv = HEV_SCGI_CONFIG_GET_PRIVATE(self);

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	if(priv->key_file)
	{
		g_key_file_free(priv->key_file);
		priv->key_file = NULL;
	}

	G_OBJECT_CLASS(hev_scgi_config_parent_class)->finalize(obj);
}

static GObject * hev_scgi_config_constructor(GType type, guint n, GObjectConstructParam * param)
{
	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);
	return G_OBJECT_CLASS(hev_scgi_config_parent_class)->constructor(type, n, param);
}

static void hev_scgi_config_constructed(GObject * obj)
{
	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);
}

static void hev_scgi_config_class_init(HevSCGIConfigClass * klass)
{
	GObjectClass * obj_class = G_OBJECT_CLASS(klass);

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	obj_class->constructor = hev_scgi_config_constructor;
	obj_class->constructed = hev_scgi_config_constructed;
	obj_class->dispose = hev_scgi_config_dispose;
	obj_class->finalize = hev_scgi_config_finalize;

	g_type_class_add_private(klass, sizeof(HevSCGIConfigPrivate));
}

static void hev_scgi_config_init(HevSCGIConfig * self)
{
	HevSCGIConfigPrivate * priv = HEV_SCGI_CONFIG_GET_PRIVATE(self);
	gchar *config_file_path = NULL;
	GError *error = NULL;

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	priv->key_file = g_key_file_new();
	if(!priv->key_file)
	  g_critical("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	config_file_path = g_build_path(G_DIR_SEPARATOR_S,
				"conf", HEV_SCGI_CONFIG_FILE_NAME, NULL);
	g_debug("Config File Path : %s", config_file_path);
	if(!g_key_file_load_from_file(priv->key_file, config_file_path,
					G_KEY_FILE_NONE, &error))
	{
		g_critical("%s:%d[%s]=>(%s)", __FILE__, __LINE__, __FUNCTION__,
					error->message);
		g_error_free(error);
	}

	g_free(config_file_path);
}

GObject * hev_scgi_config_new(void)
{
	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);
	return g_object_new(HEV_TYPE_SCGI_CONFIG, NULL);
}

GSocketAddress * hev_scgi_config_get_address(HevSCGIConfig *self)
{
	HevSCGIConfigPrivate *priv = NULL;
	GSocketAddress *socket_address = NULL;
	gchar *address = NULL;
	GError *error = NULL;

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	g_return_val_if_fail(HEV_IS_SCGI_CONFIG(self), NULL);
	priv = HEV_SCGI_CONFIG_GET_PRIVATE(self);

	address = g_key_file_get_string(priv->key_file,
				"Server", "Address", &error);
	if(!address)
	{
		g_critical("%s:%d[%s]=>(%s)", __FILE__, __LINE__, __FUNCTION__,
					error->message);
		g_error_free(error);
	}
	else
	{
		if(g_regex_match_simple("^unix:.*$", address, 0, 0))
		{
			socket_address =
				g_unix_socket_address_new(address+5);
		}
		else
		{
			gchar **addr = NULL;

			addr = g_regex_split_simple(":", address, 0, 0);
			if(addr && (2==g_strv_length(addr)))
			{
				GInetAddress *inet_address = NULL;

				inet_address = g_inet_address_new_from_string(addr[0]);
				socket_address = g_inet_socket_address_new(inet_address,
							atoi(addr[1]));

				g_object_unref(G_OBJECT(inet_address));
			}

			g_strfreev(addr);
		}

		g_free(address);
	}

	return socket_address;
}

