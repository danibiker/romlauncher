package com.marcosoft.gamescraper.exceptions;

public class HttpRangeException extends HttpCustomException {
	private static final long serialVersionUID = -8998753114842096369L;
	public HttpRangeException(String errorMessage, Throwable err) {
		super(errorMessage, err);
	}
	public HttpRangeException(String errorMessage) {
		super(errorMessage);
	}
}
