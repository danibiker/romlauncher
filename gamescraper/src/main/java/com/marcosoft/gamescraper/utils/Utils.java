package com.marcosoft.gamescraper.utils;

import java.io.File;
import java.io.InputStream;
import java.util.List;
import java.util.Locale;
import java.util.MissingResourceException;
import java.util.ResourceBundle;
import java.util.logging.Level;
import java.util.logging.Logger;

import com.marcosoft.gamescraper.App;

public class Utils {
	
	private final static Logger logger = Logger.getLogger(Utils.class.getName());
	
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
    
    public static InputStream loadResource(String path) {
    	InputStream icoStream = Thread.currentThread().getContextClassLoader().getResourceAsStream(path);
    	if (icoStream == null)
    		return Thread.currentThread().getContextClassLoader().getResourceAsStream("resources/" + path);
    	else return icoStream;
    }
    
    /**
     * 
     * @param resource
     * @param locale
     * @return
     */
    public static ResourceBundle getResourceBundle(String resource, Locale locale) {
    	boolean localeFound = false;
    	int times = 0;
    	ResourceBundle retBundle = null;
    	do {
    		try {
	    		logger.log(Level.ALL, String.format("Loading messages: %s", locale.getDisplayLanguage()));
	    		retBundle = ResourceBundle.getBundle(resource, locale);
	    		localeFound = true;
	    	} catch (MissingResourceException e) {
	    		times++;
	    		logger.log(Level.ALL, "Trying jar directory prefix");
	    		resource = "resources." + resource;
	    	}
    	} while (!localeFound && times < 2);
    	return retBundle;
    }
    
    /**
     * 
     * @return
     */
    public static ResourceBundle getDefaultOrFirstResourceBundle() {
    	boolean localeFound = false;
		Locale locale = Locale.getDefault();
		ResourceBundle rb = null;
		
    	try {
    		rb = Utils.getResourceBundle("messages", locale);
    		localeFound = true;
    	} catch (MissingResourceException e) {
    		logger.log(Level.SEVERE, "Error loading resources");
    		String [] localesStr = {"en","es"};
        	
        	int i=0;
    		do {
    			try {
    				logger.log(Level.ALL, String.format("searching for locale: %s", localesStr[i]));
					rb = Utils.getResourceBundle("messages", Locale.forLanguageTag(localesStr[i]));
					localeFound = true;
				} catch (MissingResourceException e1) {
					localeFound = false;
					logger.log(Level.SEVERE, String.format("Error Loading messages: %s", localesStr[i]));
				}
    			i++;
        	} while (!localeFound && i < localesStr.length);
    	}
    	
    	return rb;
    }

}
