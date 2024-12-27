package com.marcosoft.gamescraper.utils;

import java.io.File;
import java.util.List;

public class Utils {
	private Utils() {
	}

	/**
	 * 
	 * @param str
	 * @return
	 */
	public static int safeInt(String str) {
		try {
			return Integer.parseInt(str);
		} catch (NumberFormatException e) {
			return 0;
		}
	}
	
	/**
	 * 
	 * @param str
	 * @return
	 */
	public static String safeString(String str) {
		return str != null ? str : "";
	}
	
	/**
	 * 
	 * @param obj
	 * @return
	 */
	public static boolean isEmpty(Object obj) {
		boolean ret = obj == null;
		if (!ret && obj instanceof String) {
			ret = ((String)obj).isEmpty();
		} else if (!ret && obj instanceof List) {
			ret = ((List<?>)obj).isEmpty();
		}
		return ret;
	}
	
	/**
	 * 
	 * @param inputString
	 * @param length
	 * @param character
	 * @return
	 */
	public static String pad(String inputString, int length, char character) {
		return String.format("%1$" + length + "s", inputString).replace(' ', character);
	}
	
	/**
	 * 
	 * @param file
	 * @return
	 */
    public static String getFileNameWithoutExtension(String file) {
    	if (file.contains("\\")) {
    		file = file.substring(file.lastIndexOf("\\") + 1);
    	}
    	if (file.contains("/")) {
    		file = file.substring(file.lastIndexOf("/") + 1);
    	}
    	if (file.contains(".")) {
    		file = file.substring(0, file.lastIndexOf("."));
    	}
    	return file;
    }
    
    /**
     * 
     * @param absfilepath
     * @param prefix
     * @return
     */
    public static String getAbsolutePath(String absfilepath, String prefix) {
    	String tmpPrefix = prefix.trim() + (prefix.trim().endsWith(File.separator) ? "" : File.separator);
    	return absfilepath.trim().startsWith(File.separator) ? absfilepath : tmpPrefix + absfilepath.trim();
    }
    
    /**
     * 
     * @param base
     * @param filedir
     * @return
     */
    public static String relativize(String base, String filedir) {
    	if (filedir.contains(base)) {
			return filedir.substring(base.length() + 1);
		} else {
			return filedir;
		}
    }
    
    /**
     * 
     * @param base
     * @param filedir
     * @return
     */
    public static String relativizeForDos(String base, String filedir) {
    	if (filedir.contains(base)) {
			return filedir.substring(base.length() + 1);
		} else {
			return filedir;
		}
    }
    


}
