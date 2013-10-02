/*
 ============================================================================
 Name        : hev-scgi-response.c
 Author      : Heiher <admin@heiher.info>
 Version     : 0.0.1
 Copyright   : Copyright (C) 2011 everyone.
 Description : 
 ============================================================================
 */

#include <string.h>

#include "hev-scgi-response.h"

enum
{
	HEADER_STATUS_UNWRITE,
	HEADER_STATUS_WRITING,
	HEADER_STATUS_WRITED
};

static void hev_scgi_response_output_stream_write_async_handler(GObject *source_object,
			GAsyncResult *res, gpointer user_data);
static void hev_scgi_response_output_stream_close_async_handler(GObject *source_object,
			GAsyncResult *res, gpointer user_data);

#define HEV_SCGI_RESPONSE_GET_PRIVATE(obj)	(G_TYPE_INSTANCE_GET_PRIVATE((obj), HEV_TYPE_SCGI_RESPONSE, HevSCGIResponsePrivate))

typedef struct _HevSCGIResponsePrivate HevSCGIResponsePrivate;

struct _HevSCGIResponsePrivate
{
	GOutputStream *output_stream;
	guint header_status;
	GHashTable *header_hash_table;
	GHashTableIter header_hash_table_iter;
	gboolean last_write;
	gchar *header_buffer;
};

G_DEFINE_TYPE(HevSCGIResponse, hev_scgi_response, G_TYPE_OBJECT);

static void hev_scgi_response_dispose(GObject * obj)
{
	HevSCGIResponse * self = HEV_SCGI_RESPONSE(obj);
	HevSCGIResponsePrivate * priv = HEV_SCGI_RESPONSE_GET_PRIVATE(self);

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	if(priv->output_stream)
	{
		g_output_stream_close_async(priv->output_stream,
					0, NULL,
					hev_scgi_response_output_stream_close_async_handler,
					NULL);
	}

	G_OBJECT_CLASS(hev_scgi_response_parent_class)->dispose(obj);
}

static void hev_scgi_response_finalize(GObject * obj)
{
	HevSCGIResponse * self = HEV_SCGI_RESPONSE(obj);
	HevSCGIResponsePrivate * priv = HEV_SCGI_RESPONSE_GET_PRIVATE(self);

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	if(priv->header_buffer)
	{
		g_free(priv->header_buffer);
		priv->header_buffer = NULL;
	}

	if(priv->header_hash_table)
	{
		g_hash_table_destroy(priv->header_hash_table);
		priv->header_hash_table = NULL;
	}

	G_OBJECT_CLASS(hev_scgi_response_parent_class)->finalize(obj);
}

static GObject * hev_scgi_response_constructor(GType type, guint n, GObjectConstructParam * param)
{
	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);
	return G_OBJECT_CLASS(hev_scgi_response_parent_class)->constructor(type, n, param);
}

static void hev_scgi_response_constructed(GObject * obj)
{
	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	G_OBJECT_CLASS(hev_scgi_response_parent_class)->constructed(obj);
}

static void hev_scgi_response_class_init(HevSCGIResponseClass * klass)
{
	GObjectClass * obj_class = G_OBJECT_CLASS(klass);

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	obj_class->constructor = hev_scgi_response_constructor;
	obj_class->constructed = hev_scgi_response_constructed;
	obj_class->dispose = hev_scgi_response_dispose;
	obj_class->finalize = hev_scgi_response_finalize;

	g_type_class_add_private(klass, sizeof(HevSCGIResponsePrivate));
}

static void hev_scgi_response_init(HevSCGIResponse * self)
{
	HevSCGIResponsePrivate * priv = HEV_SCGI_RESPONSE_GET_PRIVATE(self);

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	priv->output_stream = NULL;
	priv->header_status = HEADER_STATUS_UNWRITE;

	priv->header_hash_table = g_hash_table_new_full(g_str_hash,
				g_str_equal, g_free, g_free);
	if(!priv->header_hash_table)
	  g_critical("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	priv->last_write = FALSE;
}

/**
 * hev_scgi_response_new
 *
 * Creates a response.
 *
 * Returns: (type HevSCGIResponse): A #HevSCGIResponse.
 *
 * Since: 0.0.1
 */
GObject * hev_scgi_response_new(void)
{
	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);
	return g_object_new(HEV_TYPE_SCGI_RESPONSE, NULL);
}

/**
 * hev_scgi_response_set_output_stream
 * @self: A #HevSCGIResponse
 * @output_stream: A #GOutputStream
 *
 * Sets the output stream @self is for.
 *
 * Since: 0.0.1
 */
void hev_scgi_response_set_output_stream(HevSCGIResponse *self,
			GOutputStream *output_stream)
{
	HevSCGIResponsePrivate *priv = NULL;

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);
	
	g_return_if_fail(HEV_IS_SCGI_RESPONSE(self));
	g_return_if_fail(G_IS_OUTPUT_STREAM(output_stream));
	priv = HEV_SCGI_RESPONSE_GET_PRIVATE(self);

	if(priv->output_stream)
	  g_object_unref(priv->output_stream);

	priv->output_stream = g_object_ref(output_stream);
}

/**
 * hev_scgi_response_get_output_stream
 * @self: A #HevSCGIResponse
 *
 * Gets the output stream @self is for.
 *
 * Returns: (transfer none) (type GOutputStream): A #GOutputStream owned by @self. Do not free.
 *
 * Since: 0.0.1
 */
GOutputStream * hev_scgi_response_get_output_stream(HevSCGIResponse *self)
{
	HevSCGIResponsePrivate *priv = NULL;

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);
	
	g_return_val_if_fail(HEV_IS_SCGI_RESPONSE(self), NULL);
	priv = HEV_SCGI_RESPONSE_GET_PRIVATE(self);

	return priv->output_stream;
}

/**
 * hev_scgi_response_write_header
 * @self: A #HevSCGIResponse
 *
 * Writes header #HevSCGIResponse is for.
 *
 * Since: 0.0.1
 */
gboolean hev_scgi_response_write_header(HevSCGIResponse *self)
{
	HevSCGIResponsePrivate *priv = NULL;
	gpointer key = NULL, value = NULL;
	gboolean run = TRUE;

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);
	
	g_return_val_if_fail (HEV_IS_SCGI_RESPONSE(self), FALSE);
	priv = HEV_SCGI_RESPONSE_GET_PRIVATE(self);
	g_return_val_if_fail (NULL!=priv->output_stream, FALSE);
	g_return_val_if_fail (HEADER_STATUS_UNWRITE==priv->header_status, FALSE);

	priv->header_status = HEADER_STATUS_WRITING;

	g_hash_table_iter_init (&priv->header_hash_table_iter,
				priv->header_hash_table);
	for (; run;) {
		gchar buffer[1024];
		gint i = 0, len;

		if(g_hash_table_iter_next (&priv->header_hash_table_iter,
						&key, &value)) {
			len = g_snprintf (buffer, 1024, "%s: %s\r\n", key, value);
		} else {
			len = g_snprintf (buffer, 1024, "\r\n");
			run = FALSE;
		}

		do {
			gssize ret = g_output_stream_write (priv->output_stream,
						buffer + i, len - i, NULL, NULL);
			if (-1 == ret)
			  return FALSE;
			i += ret;
		} while (i < len);
	}

	priv->header_status = HEADER_STATUS_WRITED;
	return TRUE;
}

/**
 * hev_scgi_response_write_header_async
 * @self: A #HevSCGIResponse
 * @cancellable: (allow-none): optional #GCancellable object, %NULL to ignore.
 * @callback: (scope async): callback to call when the response is satisfied
 * @user_data: (closure): User data to pass to @callback.
 *
 * Writes header #HevSCGIResponse is for.
 *
 * Since: 0.0.1
 */
void hev_scgi_response_write_header_async(HevSCGIResponse *self,
			GCancellable *cancellable, GAsyncReadyCallback callback, gpointer user_data)
{
	HevSCGIResponsePrivate *priv = NULL;
	GSimpleAsyncResult *simple = NULL;
	gpointer key = NULL, value = NULL;

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);
	
	g_return_if_fail(HEV_IS_SCGI_RESPONSE(self));
	g_return_if_fail(callback);
	priv = HEV_SCGI_RESPONSE_GET_PRIVATE(self);
	g_return_if_fail(NULL!=priv->output_stream);
	g_return_if_fail(HEADER_STATUS_UNWRITE==priv->header_status);

	/* Simple async result */
	simple = g_simple_async_result_new(G_OBJECT(self),
				callback, user_data, hev_scgi_response_write_header_async);
	g_simple_async_result_set_check_cancellable(simple, cancellable);

	priv->header_status = HEADER_STATUS_WRITING;
	g_object_set_data(G_OBJECT(self), "simple", simple);
	g_hash_table_iter_init(&priv->header_hash_table_iter,
				priv->header_hash_table);
	if(g_hash_table_iter_next(&priv->header_hash_table_iter,
					&key, &value))
	{
		priv->header_buffer = g_strdup_printf("%s: %s\r\n",
					key, value);
		g_output_stream_write_async(priv->output_stream,
					priv->header_buffer, strlen(priv->header_buffer), 0, NULL,
					hev_scgi_response_output_stream_write_async_handler,
					self);
	}
	else
	{
		g_output_stream_write_async(priv->output_stream,
					"\r\n", 2, 0, NULL,
					hev_scgi_response_output_stream_write_async_handler,
					self);
		priv->last_write = TRUE;
	}
}

/**
 * hev_scgi_response_write_header_finish
 * @self: A #HevSCGIResponse
 * @res: A #GAsyncResult
 * @error: A #GError location to store the error occurring, or %NULL to ignore.
 *
 * Finishes an asynchronous write operation.
 *
 * Returns: %TRUE if the response was write successfully.
 */
gboolean hev_scgi_response_write_header_finish(HevSCGIResponse *self, GAsyncResult *res,
			GError **error)
{
	HevSCGIResponsePrivate *priv = NULL;

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	g_return_val_if_fail(HEV_IS_SCGI_RESPONSE(self), FALSE);
	priv = HEV_SCGI_RESPONSE_GET_PRIVATE(self);

	g_return_val_if_fail(g_simple_async_result_is_valid(res,
					G_OBJECT(self), hev_scgi_response_write_header_async),
				FALSE);

	if(g_simple_async_result_propagate_error(G_SIMPLE_ASYNC_RESULT(res),
					error))
	  return FALSE;

	return g_simple_async_result_get_op_res_gboolean(G_SIMPLE_ASYNC_RESULT(res));
}

static void hev_scgi_response_output_stream_write_async_handler(GObject *source_object,
			GAsyncResult *res, gpointer user_data)
{
	HevSCGIResponse *self = HEV_SCGI_RESPONSE(user_data);
	HevSCGIResponsePrivate *priv = HEV_SCGI_RESPONSE_GET_PRIVATE(self);
	gssize size = 0;
	GError *error = NULL;
	gpointer key = NULL, value = NULL;
	GSimpleAsyncResult *simple = NULL;

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	size = g_output_stream_write_finish(G_OUTPUT_STREAM(source_object),
				res, &error);

	simple = g_object_get_data(G_OBJECT(self), "simple");
	/* Call callback when connection closed by remote or error */
	if(0 >= size)
	{
		priv->header_status = HEADER_STATUS_WRITED;

		g_simple_async_result_take_error(simple, error);
		g_simple_async_result_set_op_res_gboolean(simple, FALSE);
		g_simple_async_result_complete_in_idle(simple);
		g_object_unref(simple);

		return;
	}

	if(priv->last_write)
	{
		priv->header_status = HEADER_STATUS_WRITED;

		g_simple_async_result_set_op_res_gboolean(simple, TRUE);
		g_simple_async_result_complete(simple);
		g_object_unref(simple);
	}
	else
	{
		if(g_hash_table_iter_next(&priv->header_hash_table_iter,
						&key, &value))
		{
			if(priv->header_buffer)
			  g_free(priv->header_buffer);

			priv->header_buffer = g_strdup_printf("%s: %s\r\n",
						key, value);
			g_output_stream_write_async(priv->output_stream,
						priv->header_buffer, strlen(priv->header_buffer), 0, NULL,
						hev_scgi_response_output_stream_write_async_handler,
						self);
		}
		else
		{
			g_output_stream_write_async(priv->output_stream,
						"\r\n", 2, 0, NULL,
						hev_scgi_response_output_stream_write_async_handler,
						self);
			priv->last_write = TRUE;
		}
	}
}

static void hev_scgi_response_output_stream_close_async_handler(GObject *source_object,
			GAsyncResult *res, gpointer user_data)
{
	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	g_output_stream_close_finish(G_OUTPUT_STREAM(source_object),
				res, NULL);
	g_object_unref(source_object);
}

/**
 * hev_scgi_response_get_header_hash_table
 * @self: A #HevSCGIResponse
 *
 * Gets the header hash table @self is for.
 *
 * Returns: (transfer none) (element-type utf8 utf8): A #GHashTable owned by @self. Do not free.
 *
 * Since: 0.0.1
 */
GHashTable * hev_scgi_response_get_header_hash_table(HevSCGIResponse *self)
{
	HevSCGIResponsePrivate *priv = NULL;

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	g_return_val_if_fail(HEV_IS_SCGI_RESPONSE(self), NULL);
	priv = HEV_SCGI_RESPONSE_GET_PRIVATE(self);

	return priv->header_hash_table;
}

