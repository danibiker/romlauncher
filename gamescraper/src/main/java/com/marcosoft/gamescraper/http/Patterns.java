package com.marcosoft.gamescraper.http;

import java.util.regex.Pattern;

public class Patterns {

	private Patterns() {
	}
	
	public static final Pattern IP_ADDRESS
    = Pattern.compile(
        "((25[0-5]|2[0-4][0-9]|[0-1][0-9]{2}|[1-9][0-9]|[1-9])\\.(25[0-5]|2[0-4]"
        + "[0-9]|[0-1][0-9]{2}|[1-9][0-9]|[1-9]|0)\\.(25[0-5]|2[0-4][0-9]|[0-1]"
        + "[0-9]{2}|[1-9][0-9]|[1-9]|0)\\.(25[0-5]|2[0-4][0-9]|[0-1][0-9]{2}"
        + "|[1-9][0-9]|[0-9]))");
    /**
     * Good characters for Internationalized Resource Identifiers (IRI).
     * This comprises most common used Unicode characters allowed in IRI
     * as detailed in RFC 3987.
     * Specifically, those two byte Unicode characters are not included.
     */
    public static final String GOOD_IRI_CHAR =
        "a-zA-Z0-9\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF";
	 /**
     * RFC 1035 Section 2.3.4 limits the labels to a maximum 63 octets.
     */
    public static final String IRI
        = "[" + GOOD_IRI_CHAR + "]([" + GOOD_IRI_CHAR + "\\-]{0,61}[" + GOOD_IRI_CHAR + "]){0,1}";

    public static final String GOOD_GTLD_CHAR =
        "a-zA-Z\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF";
    public static final String GTLD = "[" + GOOD_GTLD_CHAR + "]{2,63}";
	
    public static final String HOST_NAME = "(" + IRI + "\\.)+" + GTLD;
	
	public static final Pattern DOMAIN_NAME
    = Pattern.compile("(" + HOST_NAME + "|" + IP_ADDRESS + ")");
	
	/*Pattern pattern = Pattern.compile(
			"((http|https)://)(www.)?[a-zA-Z0-9@:%._\\\\+~#?&//=]{2,256}\\\\.[a-z]{2,6}\\\\b([-a-zA-Z0-9@:%._\\\\+~#?&//=]*)",
			Pattern.CASE_INSENSITIVE);*/
	public static final Pattern WEB_URL = Pattern.compile(
	        "((?:(http|https|Http|Https|rtsp|Rtsp):\\/\\/(?:(?:[a-zA-Z0-9\\$\\-\\_\\.\\+\\!\\*\\'\\(\\)"
	        + "\\,\\;\\?\\&\\=]|(?:\\%[a-fA-F0-9]{2})){1,64}(?:\\:(?:[a-zA-Z0-9\\$\\-\\_"
	        + "\\.\\+\\!\\*\\'\\(\\)\\,\\;\\?\\&\\=]|(?:\\%[a-fA-F0-9]{2})){1,25})?\\@)?)?"
	        + "(?:" + DOMAIN_NAME + ")"
	        + "(?:\\:\\d{1,5})?)" // plus option port number
	        + "(\\/(?:(?:[" + GOOD_IRI_CHAR + "\\;\\/\\?\\:\\@\\&\\=\\#\\~"  // plus option query params
	        + "\\-\\.\\+\\!\\*\\'\\(\\)\\,\\_])|(?:\\%[a-fA-F0-9]{2}))*)?"
	        + "(?:\\b|$)"); // and finally, a word boundary or end of
	                        // input.  This is to stop foo.sure from
	                        // matching as foo.su
}
