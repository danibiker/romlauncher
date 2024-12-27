package com.marcosoft.gamescraper.http;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.BufferedReader;
import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.StringWriter;
import java.net.ConnectException;
import java.net.CookieHandler;
import java.net.CookieManager;
import java.net.HttpCookie;
import java.net.HttpURLConnection;
import java.net.MalformedURLException;
import java.net.ProtocolException;
import java.net.SocketTimeoutException;
import java.net.URI;
import java.net.URL;
import java.net.UnknownHostException;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.nio.file.StandardOpenOption;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Random;
import java.util.logging.Level;
import java.util.logging.Logger;
import java.util.zip.GZIPInputStream;

import com.marcosoft.gamescraper.exceptions.HttpCustomException;
import com.marcosoft.gamescraper.exceptions.HttpRangeException;
import com.marcosoft.gamescraper.http.HttpConst.DownloadErrors;
import com.marcosoft.gamescraper.http.HttpConst.HttpStatusCode;
import com.marcosoft.gamescraper.utils.Utils;

import lombok.Data;
import lombok.Getter;

@Getter
public class HttpUtil {
	private final Logger logger = Logger.getLogger(HttpUtil.class.getName());
	private int progress;
	private long totalBytesRead;
	private long fileSize;
	private int speedBytes;
	private ByteArrayOutputStream dataReceived;
	private Map<String, String> responseHeaders;
	private CookieManager cookieManager;
	private String cookieAddStr;
	private DownloadErrors downloadErrors;
	private HttpStatusCode httpCode;
	private Random r = new Random();
	
	public HttpUtil() {
		progress = 0;
		totalBytesRead = 0;
		speedBytes = 0;
		fileSize = 0;
		httpCode = HttpStatusCode.NOT_CONNECTED_YET;
		dataReceived = new ByteArrayOutputStream();
		responseHeaders = new HashMap<>();
		cookieManager = new CookieManager();
		cookieAddStr = "";
		CookieHandler.setDefault(cookieManager);
		downloadErrors = HttpConst.DownloadErrors.NO_ERROR;
	}
	
	@Data
	public class SpeedDownload {
		long lastTime;
		int periodBytesRead;
		int lastSpeed;
		
		SpeedDownload(){
			lastTime = System.currentTimeMillis();
			periodBytesRead = 0;
			lastSpeed = 0;
		}
	}
	

	
	/**
	 * Solo descarga una url en memoria
	 * 
	 * @param url
	 * @return
	 */
	public boolean download(String url){
	    return sendHttpWithRetries(url, "", 0, 0, null, HttpConst.Methods.HTTP_GET);
	}
	
	/**
	 * 
	 * @param urlLocation
	 * @param filepath
	 * @param headers
	 * @return
	 */
	public boolean download(String urlLocation, String filepath, Map <String, String> headers) {
		return sendHttpWithRetries(urlLocation, filepath, 0, 0, headers, HttpConst.Methods.HTTP_GET);
	}
	
	/**
	 * 
	 * @param url
	 * @param headers
	 * @return
	 */
	public boolean get(String url, Map<String, String> headers){
	    return sendHttpWithRetries(url, "", 0, 0, headers, HttpConst.Methods.HTTP_GET);
	}
	
	/**
	 * 
	 * @param url
	 * @param data
	 * @param headers
	 * @return
	 */
	public boolean post(String url, String data, Map <String, String> headers){
	    return sendHttpWithRetries(url, data, data.length(), 0, headers, HttpConst.Methods.HTTP_POST);
	}
	
	/**
	 * 
	 * @param url
	 * @param data
	 * @param tam
	 * @param offset
	 * @param headers
	 * @param httpType
	 * @return
	 */
	boolean sendHttpWithRetries(String url, String data, long tam, long offset, Map <String, String> headers, HttpConst.Methods httpType){
		boolean ret = false;
		int numTries = 0;
		do {
			try {
				ret = sendHttp(url, data, tam, offset, headers, httpType);
			} catch (SocketTimeoutException e) {
				//Try to resume the download with another connection until max connection attempts reached
				File f = new File(data);
				if (f.exists()) 
					offset = f.length();
				logger.log(Level.INFO, "{0} while receiving data in try: {1} with offset: {2} and received {3} to {4}. Error: {5}", 
						new String []{e.getClass().getName(), String.valueOf(numTries), String.valueOf(offset), 
								String.valueOf(this.totalBytesRead), url, e.getMessage()});
				numTries++; 
				this.downloadErrors = HttpConst.DownloadErrors.SOCKET_TIMEOUT;
			} catch (ConnectException e) {
				//Try another connection until max connection attempts reached
				logger.log(Level.INFO, "{0} while connecting in try {1} to {2} with http code {3}. Message: {4}", new String []{ e.getClass().getName(), 
						String.valueOf(numTries), url, String.valueOf(this.httpCode), e.getMessage()});
				numTries++; 
				this.downloadErrors = HttpConst.DownloadErrors.UNABLE_CONNECT;
			} catch (HttpRangeException e) {
				//Try another connection until max connection attempts reached
				logger.log(Level.INFO, "{0} while connecting to {1}. Message: {2}", new String []{ e.getClass().getName(), url, e.getMessage()});
				//Resetting the offset to start the download again
				offset = 0; 
				numTries++; 
				this.downloadErrors = HttpConst.DownloadErrors.SOCKET_TIMEOUT;
			} catch (IOException e) { //MalformedURLException | ProtocolException | UnknownHostException ...
				logger.log(Level.INFO, "{0} while connecting to {1} with http code {2}. Message: {3}", new String []{ e.getClass().getName(), 
						url, String.valueOf(this.httpCode), e.getMessage()});
				numTries = HttpConst.MAX_TIMEOUT_RETRIES; //In this case we don't want to retry the connection. Just exit with error
				if (e instanceof MalformedURLException) {
					this.downloadErrors = HttpConst.DownloadErrors.BAD_URL;
				} else if (e instanceof ProtocolException) {
					this.downloadErrors = HttpConst.DownloadErrors.BAD_PROTOCOL;
				} else if (e instanceof UnknownHostException) {
					this.downloadErrors = HttpConst.DownloadErrors.UNKNOWN_HOST;
				} else if (e instanceof IOException) {
					this.downloadErrors = HttpConst.DownloadErrors.IO_EXCEPTION;
				} else {
					this.downloadErrors = HttpConst.DownloadErrors.UNDEFINED;
				}
			} 
		} while (numTries < HttpConst.MAX_TIMEOUT_RETRIES && !ret);
		
		//After all, even if we had retries, and the download has been successfull, 
		//we update the status of the download with no error
		if (ret) this.downloadErrors = HttpConst.DownloadErrors.NO_ERROR;
		
		return ret;
	}
	
	/**
	 * 
	 * @param url
	 * @param data
	 * @param tam
	 * @param offset
	 * @param headers
	 * @param httpType
	 * @return
	 * @throws IOException 
	 */
	boolean sendHttp(String url, String data, long tam, long offset, Map <String, String> headers, 
			HttpConst.Methods httpType) throws IOException, HttpCustomException 
	{
		if (!Utils.isEmpty(data) && !HttpConst.Methods.HTTP_POST.equals(httpType)) {
			checkDir(data);
		}
		
		this.totalBytesRead = offset;
		
		if (offset <= 0) {
			this.progress = 0;
			this.speedBytes = 0;
			//Reseting the buffer if there is no error
			try {
				if (this.dataReceived != null)
					this.dataReceived.flush();
			} catch (IOException e) {
				logger.log(Level.WARNING, "Error flushing the receiving buffer: {0}", e.getMessage());
			} 
			this.dataReceived = new ByteArrayOutputStream();
		}
		
		//Obtainning the connection
		HttpURLConnection httpConnection = openConnection(url, data, offset, headers, httpType);
		
		logger.log(Level.CONFIG, "HTTP status code: {0}, fileSize: {1} {2}", 
				new String[] { String.valueOf(httpCode), HttpConst.df.format(this.fileSize / 1024.0 / 1024.0) , "Mbytes"});
		
		this.parserHeaders(httpConnection);
		this.loadResponseCookies(httpConnection, this.cookieManager);
		
		SpeedDownload speedDownload = new SpeedDownload(); 
		boolean ret = false;
		
		if (!Utils.isEmpty(data) && !HttpConst.Methods.HTTP_POST.equals(httpType)) {
			//Writes the data directly to a File in the local hard disk 
			try (BufferedInputStream in = new BufferedInputStream(httpConnection.getInputStream());
					BufferedOutputStream outputStream = new BufferedOutputStream(new FileOutputStream(data, offset > 0), 512 * 1024)) {
				ret = writeToByteArray(speedDownload, in, outputStream);
			} catch (IOException e) {
				httpConnection.disconnect();
				throw e;
			}
		} else {
			//Writes the data into a memory buffer
			try (BufferedInputStream in = (new BufferedInputStream(httpConnection.getInputStream()))){
				ret = writeToByteArray(speedDownload, in, dataReceived);
			} catch (SocketTimeoutException e) {
				//We don't have a file to obtain it's offset, so we set it to the total bytes read yet
				offset = totalBytesRead;
				httpConnection.disconnect();
				throw e;
			} catch (IOException e) {
				httpConnection.disconnect();
				throw e;
			}
		}
		
		String encoding = responseHeaders.get("Content-Encoding");
		if (encoding != null && encoding.toLowerCase().contains("gzip")) {
			logger.log(Level.FINER, "decompressing the response");
			this.decompressGzip(dataReceived);
		}
	    httpConnection.disconnect();
	    return ret;
	}

	/**
	 * 
	 * @param speedDownload
	 * @param in
	 * @param out
	 * @return
	 * @throws IOException
	 */
	private boolean writeToByteArray(SpeedDownload speedDownload, BufferedInputStream in, OutputStream out) throws IOException {
		byte[] buffer = new byte[HttpConst.DOWNLOAD_CHUNK_BYTES];
		int bytesRead = 0;
		while ((bytesRead = in.read(buffer, 0, HttpConst.DOWNLOAD_CHUNK_BYTES)) != -1){
			totalBytesRead += bytesRead;
			if (fileSize > 0) {
		    	progress = (int) (totalBytesRead / (float)fileSize * 100.0);
		    } 
			out.write(buffer, 0, bytesRead);
			speedBytes = computeSpeed(speedDownload, bytesRead);
		}
		logger.log(Level.FINEST, "writeToByteArray totalBytesRead: {0}", totalBytesRead);
		progress = 100;
		return true;
	}
	
	/**
	 * 
	 * @param urlLocation
	 * @param data
	 * @param offset
	 * @param headers
	 * @param httpType
	 * @return
	 * @throws IOException 
	 */
	private HttpURLConnection openConnection(String urlLocation, String data, long offset, 
			Map <String, String> headers, HttpConst.Methods httpType) throws IOException, HttpCustomException 
	{
		HttpURLConnection httpConnection;
		URL url = URI.create(urlLocation).toURL();
		httpConnection = (HttpURLConnection) url.openConnection();
		
		//To allow resume a connection
		if (offset > 0) {
			httpConnection.setRequestProperty("Range", "bytes="+offset+"-");
		}
		
		//Obtaing the cookies and add extra data
		String cookie = populateCookieHeaders(cookieManager);
		if (!cookie.isEmpty()) {
			httpConnection.setRequestProperty(HttpConst.COOKIE_REQUEST_HEADER, cookie 
					+ (cookieAddStr != null && !cookieAddStr.isEmpty() ? cookieAddStr : ""));
		}
		
		//Setting a default user-agent if not existant
		if (headers == null || !headers.containsKey("User-Agent"))
			httpConnection.setRequestProperty ("User-Agent", HttpConst.USERAGENT);
		
		if (headers != null) {
			for (Map.Entry<String,String> entry : headers.entrySet()) {
				httpConnection.setRequestProperty( entry.getKey(), entry.getValue());
			}
		}
		httpConnection.setConnectTimeout(HttpConst.CONN_TIMEOUT);
		httpConnection.setReadTimeout(HttpConst.CONN_READ_TIMEOUT);
		httpConnection.setRequestMethod(httpType.getText());
		httpConnection.setDoInput(true);
		
		if (httpType == HttpConst.Methods.HTTP_POST) {
			httpConnection.setDoOutput(true);
			if (headers == null || !headers.containsKey(HttpConst.CONTENT_LENGTH)) {
				httpConnection.setRequestProperty(HttpConst.CONTENT_LENGTH, Integer.toString(data.length()));
			}
			addDefaultHeaders(headers, httpConnection);
			try(OutputStream os = httpConnection.getOutputStream()) {
			    byte[] input = data.getBytes(StandardCharsets.UTF_8);
			    os.write(input, 0, input.length);			
			}
		} else {
			HttpURLConnection.setFollowRedirects(true);
			httpConnection.setInstanceFollowRedirects(true);  //you still need to handle redirect manully.
		}
		//Storing the response code. It does connect
		httpCode = HttpStatusCode.getByValue(httpConnection.getResponseCode());
		
		//416 Range Not Satisfiable. We tried to resume a download but the server denied it
		//So, we reset the offset to try again consuming one more of the total tryings, wich
		//is done in the sendHttpWithRetries method, capturing this thrown exception
		if (offset > 0 && HttpStatusCode.REQUESTED_RANGE_NOT_SATISFIABLE.equals(httpCode)) {
			throw new HttpRangeException("HTTP error 416 Range Not Satisfiable -> " + urlLocation);
		}
		
		//Getting the file size
		if ((fileSize = httpConnection.getContentLength()) <= 0) {
			fileSize = httpConnection.getHeaderFieldLong(HttpConst.CONTENT_LENGTH, 0);
		}
		
		//Handling the size of the file taking into account the 206 Partial Content header
		handlePartialContentLength(offset, httpConnection);
		
		return httpConnection;
	}

	/**
	 * @param offset
	 * @param httpConnection
	 */
	private void handlePartialContentLength(long offset, HttpURLConnection httpConnection) {
		//In case of a resumed connection, we must add the offset to response content-length.
		//Maybe it's better to obtain the header "Content-Range: bytes 21010-47021/47022"
		//if the response code is a "206 Partial Content"
		String contentRangeRecv = httpConnection.getHeaderField("Content-Range");
		boolean acceptRanges = ("bytes").equals(httpConnection.getHeaderField("Accept-Ranges"));
		
		int pos = 0;
		long totalContentLengthRecv = 0;
		if (HttpStatusCode.PARTIAL_CONTENT.equals(httpCode) && !Utils.isEmpty(contentRangeRecv) 
				&& (pos = contentRangeRecv.lastIndexOf("/")) > 0 && pos < contentRangeRecv.length() - 1) 
		{
			try {
				logger.log(Level.INFO, "HTTP 206 Partial Content received: {0} and accept ranges: {1}", 
						new String [] {contentRangeRecv , acceptRanges ? "Yes" : "No"});
				totalContentLengthRecv = Long.parseLong(contentRangeRecv.substring(pos + 1));
			} catch (NumberFormatException e) {
				logger.log(Level.INFO, "Content-Range {0} received, but could not extract the content length", contentRangeRecv);
			}
		}
		
		if (totalContentLengthRecv > 0) {
			this.fileSize = totalContentLengthRecv;
		} else if (offset > 0 && fileSize > 0) {
			this.fileSize += offset;
		}
	}

	/**
	 * Gets Cookies from the response header and loads them into cookie manager
	 *
	 * @param conn          instance of {@link HttpURLConnection} object
	 * @param cookieManager the cookie manager({@link CookieManager} instance) in which the cookies are to be loaded<p>In case a null object is passed, the function will not perform any action and return back to the caller. </p>
	 */
	
	public void loadResponseCookies(HttpURLConnection conn, CookieManager cookieManager) {

	    //do nothing in case a null cokkie manager object is passed
	    if (cookieManager == null || conn == null){
	        return;
	    }

	    List<String> cookiesHeader = conn.getHeaderFields().get(HttpConst.COOKIES_HEADER);
	    if (cookiesHeader != null) {
	        for (String cookieHeader : cookiesHeader) {
	            List<HttpCookie> cookies;
	            try {
	                cookies = HttpCookie.parse(cookieHeader);
	            } catch (NullPointerException | IllegalArgumentException e) {
	            	logger.log(Level.FINEST, "{0} -- Null header for the cookie : {1}", new String [] {conn.getURL().toString(), cookieHeader.toString()});
	                //ignore the Null cookie header and proceed to the next cookie header
	                continue;
	            }

	            if (cookies != null) {
	            	logger.log(Level.FINEST, "{0} -- Reading Cookies from the response :", conn.getURL());
	                if (logger.isLoggable(Level.FINEST)) {
	                	for (HttpCookie cookie : cookies) {
		                	logger.log(Level.FINEST, cookie.toString());
		                }
	                }
	                if (!cookies.isEmpty()) {
	                    cookieManager.getCookieStore().add(null, HttpCookie.parse(cookieHeader).get(0));
	                }
	            }
	        }
	    }
	}
	
	/**
	 * 
	 * @param cookieManager
	 * @return
	 */
	public String populateCookieHeaders(CookieManager cookieManager) {
		StringWriter sw = new StringWriter();
	    if (cookieManager != null) {
	        //getting cookies(if any) and manually adding them to the request header
	        List<HttpCookie> cookies = cookieManager.getCookieStore().getCookies();
	        if (cookies != null && !cookies.isEmpty()) {
            	logger.log(Level.FINEST, "-- Adding Cookie Headers");
            	List<String> cookiesAdded = new ArrayList<>();
                for (int i=cookies.size()-1; i >= 0; i--) {
                	if (!cookiesAdded.contains(cookies.get(i).getName())) {
                		logger.log(Level.FINEST, "FROM COOKIESTORE - i: {0} -> {1}", new String [] {String.valueOf(i), cookies.get(i).toString()});
                		if (!cookiesAdded.isEmpty()) sw.append("; ");
                		sw.append(cookies.get(i).toString());
                		cookiesAdded.add(cookies.get(i).getName());
                	}
                }
            	logger.log(Level.FINEST, "Cookie to send: {0}", sw);
                //adding the cookie header
	        }
	    }
	    return sw.toString();
	}
	
	/**
	 * @param headers
	 * @param httpConnection
	 */
	private void addDefaultHeaders(Map<String, String> headers, HttpURLConnection httpConnection) {
		if (headers == null || !headers.containsKey("Accept")) {
			httpConnection.setRequestProperty("Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.7");
		}
		if (headers == null || !headers.containsKey("Accept-Encoding")) {
			httpConnection.setRequestProperty("Accept-Encoding", "deflate");
		}
		if (headers == null || !headers.containsKey("Accept-Language")) {
			httpConnection.setRequestProperty("Accept-Language", "es-ES,es;q=0.9,en-GB;q=0.8,en;q=0.7,ca-ES;q=0.6,ca;q=0.5");
		}
		if (headers == null || !headers.containsKey("Content-Type")) {
			httpConnection.setRequestProperty("Content-Type", "application/x-www-form-urlencoded");
		}
	}
	
	/**
	 * 
	 * @param arrayCompressed
	 */
	public void decompressGzip(ByteArrayOutputStream arrayCompressed){
        try (GZIPInputStream gis = new GZIPInputStream(new ByteArrayInputStream(arrayCompressed.toByteArray()))) {
        	ByteArrayOutputStream decompressed = new ByteArrayOutputStream();
            byte[] buffer = new byte[HttpConst.DOWNLOAD_CHUNK_BYTES];
            int len;
            while ((len = gis.read(buffer)) > 0) {
            	decompressed.write(buffer, 0, len);
            }
            arrayCompressed.reset();
            arrayCompressed.write(decompressed.toByteArray());
            decompressed.reset();
        } catch (Exception e) {
        	logger.log(Level.INFO, "decompressGzip could not be decompressed {0}", e.getMessage());
		}
    }
	
	/**
	 * Obtains the response headers and saves a map with its values
	 * 
	 * @param conn
	 */
	void parserHeaders(HttpURLConnection conn){
		responseHeaders.clear();
		//get all response headers
	    Map<String, List<String>> map = conn.getHeaderFields();
	    for (Map.Entry<String, List<String>> entry : map.entrySet()) {
	    	responseHeaders.put(entry.getKey(), entry.getValue() != null && !entry.getValue().isEmpty() ? entry.getValue().get(0) : "");
	    }
	}
	
	/**
	 * 
	 * @param speedDownload
	 * @param bytesRead
	 * @return
	 */
	private int computeSpeed(SpeedDownload speedDownload, int bytesRead) {
		if (System.currentTimeMillis() - speedDownload.getLastTime() < 1000) {
			speedDownload.setPeriodBytesRead(speedDownload.getPeriodBytesRead() + bytesRead);
			return speedDownload.lastSpeed == 0 ? speedDownload.getPeriodBytesRead() : speedDownload.lastSpeed;
    	} else {
    		int speed = speedDownload.getPeriodBytesRead();
    		speedDownload.setPeriodBytesRead(0);
    		speedDownload.setLastTime(System.currentTimeMillis());
    		speedDownload.lastSpeed = (int)((speed + speedDownload.lastSpeed) / 2.0);
    		return speedDownload.lastSpeed;
    	}
	}

	/**
	 * @param filepath
	 */
	private void checkDir(String filepath) {
		File dstFile = null;
		
		if (filepath == null || filepath.isEmpty())
			return;
		
	    // check the directory for existence.
	    String dstFolder = filepath.lastIndexOf(File.separator) >= 0 ? filepath.substring(0, filepath.lastIndexOf(File.separator)) : filepath;
	    if(!(dstFolder.endsWith(File.separator) || dstFolder.endsWith("/")))
	        dstFolder += File.separator;

	    // Creates the destination folder if doesn't not exists
	    dstFile = new File(dstFolder);
	    if (!dstFile.exists()) {
	        dstFile.mkdirs();
	    }
	}
	
	public boolean writeToFile(String path){
	    return writeToFile(path, false);
	}
	
	boolean writeToFile(String uri, boolean append){
//		try (PrintWriter out = new PrintWriter(new BufferedWriter(new FileWriter(uri, append)))){
//		    out.print(dataReceived);
//		} catch (IOException e) {
//			logger.log(Level.FINEST, "IOException {0}", e.getMessage());
//			return false;
//		} finally {
//			closeDataReceived();
//		}
		
		try {
			Files.write(Paths.get(uri), dataReceived.toByteArray(), append ? StandardOpenOption.APPEND : StandardOpenOption.CREATE_NEW);
		} catch (IOException e) {
			logger.log(Level.FINEST, "IOException {0}", uri);
			return false;
		} finally {
			closeDataReceived();
		}
		return true;
	}
	
	
	
	/**
	 * 
	 */
	public void closeDataReceived() {
		try {
			if (dataReceived != null) {
				dataReceived.flush();
				dataReceived.close();
				dataReceived.reset();
			}
			if (responseHeaders != null) {
				responseHeaders.clear();
			}
		} catch (IOException e) {
			logger.log(Level.INFO, "IOException while flushing");
		}
	}
	
	/**
	 * 
	 * @param url
	 * @return
	 */
	public static boolean isValidURL(String url){
		return !Utils.isEmpty(url) && Patterns.WEB_URL.matcher(url).matches();
	}
	
	/**
	 * 
	 * @param cookieAddStr
	 */
	public void setCookieAddStr(String cookieAddStr) {
		this.cookieAddStr = cookieAddStr;
	}
	
	/**
	 * 
	 * @param compressed
	 * @return
	 */
	public static String decompress(ByteArrayOutputStream out){
		
		final StringBuilder outStr = new StringBuilder();
		try {
			final byte[] compressed = out.toByteArray();
			if ((compressed == null) || (compressed.length == 0)) {
				return "";
			}
			if (isCompressed(compressed)) {
				final GZIPInputStream gis = new GZIPInputStream(new ByteArrayInputStream(compressed));
				final BufferedReader bufferedReader = new BufferedReader(new InputStreamReader(gis, StandardCharsets.UTF_8));

				String line;
				while ((line = bufferedReader.readLine()) != null) {
					outStr.append(line);
				}
			} else {
				return out.toString(StandardCharsets.UTF_8);
			}
		} catch (IOException e) {
			return out.toString(StandardCharsets.UTF_8);
		}
		return outStr.toString();
	}

	public static boolean isCompressed(final byte[] compressed) {
	    return (compressed[0] == (byte) (GZIPInputStream.GZIP_MAGIC)) && (compressed[1] == (byte) (GZIPInputStream.GZIP_MAGIC >> 8));
	}
}
