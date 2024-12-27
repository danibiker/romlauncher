package com.marcosoft.gamescraper.http;

import java.text.DecimalFormat;

public class HttpConst {
	public enum Methods{
		HTTP_POST("POST"), 
		HTTP_GET("GET"), 
		HTTP_PUT("PUT"), 
		HTTP_DELETE("DELETE"), 
		HTTP_POST2("POST2"), 
		HTTP_POST3("POST3");
		
		private final String text;
		Methods (String text) { 
			this.text = text;
	    }
		public String getText() {
			return text;
		}
	}
	
	public enum DownloadErrors{
		NO_ERROR("NO_ERROR", "Sin errores"), 
		BAD_URL("BAD_URL", "Error URL"), 
		BAD_PROTOCOL("BAD_PROTOCOL", "Error protocolo"), 
		UNKNOWN_HOST("UNKNOWN_HOST", "Host desconocido"),
		UNABLE_CONNECT("UNABLE_CONNECT", "Error conexión"), 
		SOCKET_TIMEOUT("SOCKET_TIMEOUT", "Error timeout"),
		IO_EXCEPTION("IO_EXCEPTION", "Error de Conexión E/S"),
		UNDEFINED("UNDEFINED", "Error indefinido"),;
		
		private final String text;
		private final String readableText;
		
		DownloadErrors (String text, String readableText) { 
			this.text = text;
			this.readableText = readableText;
	    }
		public String getText() {
			return text;
		}
		public String getReadableText() {
			return readableText;
		}
	}
	
	public enum HttpStatusCode {
		
		//CUSTOM VALUES
		UNDEFINED(-1, "Undefined"),
		NOT_CONNECTED_YET(0, "Not connected"),
		
		
	    //1xx: Informational
	    CONTINUE(100, "Continue"),
	    SWITCHING_PROTOCOLS(101, "Switching Protocols"),
	    PROCESSING(102, "Processing"),
	    EARLY_HINTS(103, "Early Hints"),

	    //2xx: Success
	    OK(200, "OK"),
	    CREATED(201, "Created"),
	    ACCEPTED(202, "Accepted"),
	    NON_AUTHORITATIVE_INFORMATION(203, "Non-Authoritative Information"),
	    NO_CONTENT(204, "No Content"),
	    RESET_CONTENT(205, "Reset Content"),
	    PARTIAL_CONTENT(206, "Partial Content"),
	    MULTI_STATUS(207, "Multi-Status"),
	    ALREADY_REPORTED(208, "Already Reported"),
	    IM_USED(226, "IM Used"),

	    //3xx: Redirection
	    MULTIPLE_CHOICES(300, "Multiple Choice"),
	    MOVED_PERMANENTLY(301, "Moved Permanently"),
	    FOUND(302, "Found"),
	    SEE_OTHER(303, "See Other"),
	    NOT_MODIFIED(304, "Not Modified"),
	    USE_PROXY(305, "Use Proxy"),
	    TEMPORARY_REDIRECT(307, "Temporary Redirect"),
	    PERMANENT_REDIRECT(308, "Permanent Redirect"),

	    //4xx: Client Error
	    BAD_REQUEST(400, "Bad Request"),
	    UNAUTHORIZED(401, "Unauthorized"),
	    PAYMENT_REQUIRED(402, "Payment Required"),
	    FORBIDDEN(403, "Forbidden"),
	    NOT_FOUND(404, "Not Found"),
	    METHOD_NOT_ALLOWED(405, "Method Not Allowed"),
	    NOT_ACCEPTABLE(406, "Not Acceptable"),
	    PROXY_AUTHENTICATION_REQUIRED(407, "Proxy Authentication Required"),
	    REQUEST_TIMEOUT(408, "Request Timeout"),
	    CONFLICT(409, "Conflict"),
	    GONE(410, "Gone"),
	    LENGTH_REQUIRED(411, "Length Required"),
	    PRECONDITION_FAILED(412, "Precondition Failed"),
	    REQUEST_TOO_LONG(413, "Payload Too Large"),
	    REQUEST_URI_TOO_LONG(414, "URI Too Long"),
	    UNSUPPORTED_MEDIA_TYPE(415, "Unsupported Media Type"),
	    REQUESTED_RANGE_NOT_SATISFIABLE(416, "Range Not Satisfiable"),
	    EXPECTATION_FAILED(417, "Expectation Failed"),
	    MISDIRECTED_REQUEST(421, "Misdirected Request"),
	    UNPROCESSABLE_ENTITY(422, "Unprocessable Entity"),
	    LOCKED(423, "Locked"),
	    FAILED_DEPENDENCY(424, "Failed Dependency"),
	    TOO_EARLY(425, "Too Early"),
	    UPGRADE_REQUIRED(426, "Upgrade Required"),
	    PRECONDITION_REQUIRED(428, "Precondition Required"),
	    TOO_MANY_REQUESTS(429, "Too Many Requests"),
	    REQUEST_HEADER_FIELDS_TOO_LARGE(431, "Request Header Fields Too Large"),
	    UNAVAILABLE_FOR_LEGAL_REASONS(451, "Unavailable For Legal Reasons"),

	    //5xx: Server Error
	    INTERNAL_SERVER_ERROR(500, "Internal Server Error"),
	    NOT_IMPLEMENTED(501, "Not Implemented"),
	    BAD_GATEWAY(502, "Bad Gateway"),
	    SERVICE_UNAVAILABLE(503, "Service Unavailable"),
	    GATEWAY_TIMEOUT(504, "Gateway Timeout"),
	    HTTP_VERSION_NOT_SUPPORTED(505, "HTTP Version Not Supported"),
	    VARIANT_ALSO_NEGOTIATES(506, "Variant Also Negotiates"),
	    INSUFFICIENT_STORAGE(507, "Insufficient Storage"),
	    LOOP_DETECTED(508, "Loop Detected"),
	    NOT_EXTENDED(510, "Not Extended"),
	    NETWORK_AUTHENTICATION_REQUIRED(511, "Network Authentication Required");

	    private final int value;
	    private final String description;

	    HttpStatusCode(int value, String description) {
	        this.value = value;
	        this.description = description;
	    }

	    public int getValue() {
	        return value;
	    }

	    public String getDescription() {
	        return description;
	    }

	    @Override
	    public String toString() {
	        return value + " " + description;
	    }

	    public static HttpStatusCode getByValue(int value) {
	        for(HttpStatusCode status : values()) {
	            if(status.value == value) return status;
	        }
	        return UNDEFINED;
	    }
	}
	
	public static final int MAX_TIMEOUT_RETRIES = 5;
	public static final String CONTENT_LENGTH = "Content-Length";
	public static final int DOWNLOAD_CHUNK_BYTES = 1024 * 512; //512 KB  
	public static final int CONN_TIMEOUT = 10000;
	public static final int CONN_READ_TIMEOUT = 10000;
	public static final String COOKIES_HEADER = "Set-Cookie";
	public static final String COOKIE_REQUEST_HEADER = "Cookie";
	public static final String USERAGENT = "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:74.0) Gecko/20100101 Firefox/74.0";
	public static final DecimalFormat df = new DecimalFormat("0.00");
}
