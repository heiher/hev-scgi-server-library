/*
 ============================================================================
 Name        : hev-scgi-request.c
 Author      : Heiher <admin@heiher.info>
 Version     : 0.0.1
 Copyright   : Copyright (C) 2011 everyone.
 Description : 
 ============================================================================
 */

#include <string.h>

#include "hev-scgi-request.h"

enum
{
	HEADER_STATUS_UNREAD,
	HEADER_STATUS_READING,
	HEADER_STATUS_READED
};

static void hev_scgi_request_header_buffer_alloc(HevSCGIRequest *self,
			gsize size);
static void hev_scgi_request_input_stream_read_async_handler(GObject *source_object,
			GAsyncResult *res, gpointer user_data);
static void hev_scgi_request_input_stream_close_async_handler(GObject *source_object,
			GAsyncResult *res, gpointer user_data);
static void hev_scgi_request_parse_header(HevSCGIRequest *self);

#define HEV_SCGI_REQUEST_GET_PRIVATE(obj)	(G_TYPE_INSTANCE_GET_PRIVATE((obj), HEV_TYPE_SCGI_REQUEST, HevSCGIRequestPrivate))

typedef struct _HevSCGIRequestPrivate HevSCGIRequestPrivate;

struct _HevSCGIRequestPrivate
{
	GInputStream *input_stream;
	guint header_status;
	gchar *header_buffer;
	gsize header_buffer_size;
	gsize header_buffer_handle_size;
	gsize header_size;
	guint header_head_size;
	gboolean zero_read;
	GHashTable *header_hash_table;
};

G_DEFINE_TYPE(HevSCGIRequest, hev_scgi_request, G_TYPE_OBJECT);

static void hev_scgi_request_dispose(GObject * obj)
{
	HevSCGIRequest * self = HEV_SCGI_REQUEST(obj);
	HevSCGIRequestPrivate * priv = HEV_SCGI_REQUEST_GET_PRIVATE(self);

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	if(priv->input_stream)
	{
		g_input_stream_close_async(priv->input_stream,
					0, NULL,
					hev_scgi_request_input_stream_close_async_handler,
					NULL);
	}

	G_OBJECT_CLASS(hev_scgi_request_parent_class)->dispose(obj);
}

static void hev_scgi_request_finalize(GObject * obj)
{
	HevSCGIRequest * self = HEV_SCGI_REQUEST(obj);
	HevSCGIRequestPrivate * priv = HEV_SCGI_REQUEST_GET_PRIVATE(self);

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	if(priv->header_hash_table)
	{
		g_hash_table_destroy(priv->header_hash_table);
		priv->header_hash_table = NULL;
	}

	if(priv->header_buffer)
	{
		g_slice_free1(priv->header_buffer_size,
					priv->header_buffer);
		priv->header_buffer = NULL;
		priv->header_buffer_size = 0;
	}

	G_OBJECT_CLASS(hev_scgi_request_parent_class)->finalize(obj);
}

static GObject * hev_scgi_request_constructor(GType type, guint n, GObjectConstructParam * param)
{
	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);
	return G_OBJECT_CLASS(hev_scgi_request_parent_class)->constructor(type, n, param);
}

static void hev_scgi_request_constructed(GObject * obj)
{
	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);
}

static void hev_scgi_request_class_init(HevSCGIRequestClass * klass)
{
	GObjectClass * obj_class = G_OBJECT_CLASS(klass);

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	obj_class->constructor = hev_scgi_request_constructor;
	obj_class->constructed = hev_scgi_request_constructed;
	obj_class->dispose = hev_scgi_request_dispose;
	obj_class->finalize = hev_scgi_request_finalize;

	g_type_class_add_private(klass, sizeof(HevSCGIRequestPrivate));
}

static void hev_scgi_request_init(HevSCGIRequest * self)
{
	HevSCGIRequestPrivate * priv = HEV_SCGI_REQUEST_GET_PRIVATE(self);

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	priv->input_stream = NULL;
	priv->header_status = HEADER_STATUS_UNREAD;

	priv->header_buffer = NULL;
	priv->header_buffer_size = 0;
	priv->header_buffer_handle_size = 0;
	priv->header_size = 0;
	priv->header_head_size = 0;
	priv->zero_read = TRUE;

	priv->header_hash_table = g_hash_table_new(g_str_hash,
				g_str_equal);
	if(!priv->header_hash_table)
	  g_critical("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);
}

GObject * hev_scgi_request_new(void)
{
	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);
	return g_object_new(HEV_TYPE_SCGI_REQUEST, NULL);
}

void hev_scgi_request_set_input_stream(HevSCGIRequest *self,
			GInputStream *input_stream)
{
	HevSCGIRequestPrivate *priv = NULL;

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);
	
	g_return_if_fail(HEV_IS_SCGI_REQUEST(self));
	g_return_if_fail(G_IS_INPUT_STREAM(input_stream));
	priv = HEV_SCGI_REQUEST_GET_PRIVATE(self);

	if(priv->input_stream)
	  g_object_unref(priv->input_stream);

	priv->input_stream = g_object_ref(input_stream);
}

GInputStream * hev_scgi_request_get_input_stream(HevSCGIRequest *self)
{
	HevSCGIRequestPrivate *priv = NULL;

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);
	
	g_return_val_if_fail(HEV_IS_SCGI_REQUEST(self), NULL);
	priv = HEV_SCGI_REQUEST_GET_PRIVATE(self);

	return priv->input_stream;
}

void hev_scgi_request_read_header(HevSCGIRequest *self,
			GFunc callback, gpointer user_data)
{
	HevSCGIRequestPrivate *priv = NULL;

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);
	
	g_return_if_fail(HEV_IS_SCGI_REQUEST(self));
	g_return_if_fail(callback);
	priv = HEV_SCGI_REQUEST_GET_PRIVATE(self);
	g_return_if_fail(NULL!=priv->input_stream);
	g_return_if_fail(HEADER_STATUS_UNREAD==priv->header_status);

	priv->header_status = HEADER_STATUS_READING;
	g_object_set_data(G_OBJECT(self), "callback", callback);
	g_object_set_data(G_OBJECT(self), "user_data", user_data);
	hev_scgi_request_header_buffer_alloc(self, 4096);

	priv->header_size = 16;
	g_input_stream_read_async(priv->input_stream,
				priv->header_buffer, priv->header_size, 0, NULL,
				hev_scgi_request_input_stream_read_async_handler,
				self);
}

static void hev_scgi_request_header_buffer_alloc(HevSCGIRequest *self,
			gsize size)
{
	HevSCGIRequestPrivate *priv = NULL;
	gpointer buffer = NULL;

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);
	
	g_return_if_fail(HEV_IS_SCGI_REQUEST(self));
	g_return_if_fail(0<size);
	priv = HEV_SCGI_REQUEST_GET_PRIVATE(self);

	if(0 == (size%4096))
	  size = (size/4096) * 4096;
	else
	  size = ((size/4096)+1) * 4096;

	buffer = g_slice_alloc0(size);

	if(priv->header_buffer)
	{
		if(buffer)
		  g_memmove(buffer, priv->header_buffer,
					  priv->header_buffer_handle_size);

		g_slice_free1(priv->header_buffer_size,
					priv->header_buffer);
	}
	
	if(buffer)
	{
		priv->header_buffer = buffer;
		priv->header_buffer_size = size;
	}
}

static void hev_scgi_request_input_stream_read_async_handler(GObject *source_object,
			GAsyncResult *res, gpointer user_data)
{
	HevSCGIRequest *self = HEV_SCGI_REQUEST(user_data);
	HevSCGIRequestPrivate *priv = HEV_SCGI_REQUEST_GET_PRIVATE(self);
	gssize size = 0;
	GFunc callback = g_object_get_data(G_OBJECT(self), "callback");
	gpointer data = g_object_get_data(G_OBJECT(self), "user_data");

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	size = g_input_stream_read_finish(G_INPUT_STREAM(source_object),
				res, NULL);
	priv->header_buffer_handle_size += size;

	/* Call callback when connection closed by remote or error */
	if(0 >= size)
	{
		priv->header_status = HEADER_STATUS_READED;
		callback(self, data);
		return;
	}

	/* Continue read until enough */
	if(priv->header_buffer_handle_size < priv->header_size)
	{
		g_input_stream_read_async(priv->input_stream,
					priv->header_buffer+priv->header_buffer_handle_size,
					priv->header_size-priv->header_buffer_handle_size,
					0, NULL,
					hev_scgi_request_input_stream_read_async_handler,
					self);
		return;
	}

	if(priv->zero_read)
	{
		if(g_regex_match_simple("\\d*:", priv->header_buffer, 0, 0))
		{
			gchar ** strs = NULL;

			strs = g_regex_split_simple(":", priv->header_buffer, 0, 0);

			priv->header_head_size = strlen(strs[0]) + 1;
			priv->header_size = atoi(strs[0]) + priv->header_head_size + 1;
			if(priv->header_buffer_size < priv->header_size)
			  hev_scgi_request_header_buffer_alloc(self, priv->header_size);

			g_strfreev(strs);

			if(priv->header_buffer_handle_size < priv->header_size)
			{
				g_input_stream_read_async(priv->input_stream,
							priv->header_buffer+priv->header_buffer_handle_size,
							priv->header_size-priv->header_buffer_handle_size,
							0, NULL,
							hev_scgi_request_input_stream_read_async_handler,
							self);
				return;
			}
		}
		else /* Invalid request, just callback */
		{
			priv->header_status = HEADER_STATUS_READED;
			callback(self, data);
			return;
		}

		priv->zero_read = FALSE;
	}

	hev_scgi_request_parse_header(self);
	priv->header_status = HEADER_STATUS_READED;
	callback(self, data);
}

static void hev_scgi_request_input_stream_close_async_handler(GObject *source_object,
			GAsyncResult *res, gpointer user_data)
{
	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	g_input_stream_close_finish(G_INPUT_STREAM(source_object),
				res, NULL);
	g_object_unref(source_object);
}

static void hev_scgi_request_parse_header(HevSCGIRequest *self)
{
	HevSCGIRequestPrivate *priv = HEV_SCGI_REQUEST_GET_PRIVATE(self);
	guint i = 0;
	gchar *key = NULL;
	gchar *value = NULL;
	gboolean record = TRUE;

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	for(i=priv->header_head_size; i<priv->header_size; i++)
	{
		gboolean con = FALSE;

		if(0 == priv->header_buffer[i])
		{
			if(!record)
			{
				record = TRUE;
				continue;
			}
			else
			{
				con = TRUE;
			}
		}

		if(!record)
		  continue;

		if(key)
		{
			value = priv->header_buffer + i; 

			g_hash_table_insert(priv->header_hash_table,
						key, value);

			key = NULL;
			value = NULL;
		}
		else
		{
			key = priv->header_buffer + i;
		}

		record = con;
	}
}

GHashTable * hev_scgi_request_get_header_hash_table(HevSCGIRequest *self)
{
	HevSCGIRequestPrivate *priv = NULL;

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	g_return_val_if_fail(HEV_IS_SCGI_REQUEST(self), NULL);
	priv = HEV_SCGI_REQUEST_GET_PRIVATE(self);

	return priv->header_hash_table;
}

