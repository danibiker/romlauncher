package com.marcosoft.gamescraper.exceptions;

public class HttpCustomException extends RuntimeException {
	private static final long serialVersionUID = -6596533457410590277L;
	public HttpCustomException(String errorMessage, Throwable err) {
		super(errorMessage, err);
	}
	
	public HttpCustomException(String errorMessage) {
		super(errorMessage);
	}
}
