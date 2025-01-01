package com.marcosoft.gamescraper.utils;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.io.Writer;
import java.lang.reflect.Field;
import java.nio.charset.StandardCharsets;
import java.util.Arrays;
import java.util.List;
import java.util.Map;
import java.util.ResourceBundle;
import java.util.logging.Level;
import java.util.logging.Logger;
import java.util.stream.Collectors;

import javax.swing.JOptionPane;

import com.marcosoft.gamescraper.beans.TabElementEmuCfg;

public class CfgFile {
	
	private CfgFile() {
        
	}
	
	private static final Logger logger = Logger.getLogger(CfgFile.class.getName());
	
	/**
	 * 
	 * @param cfgFile
	 * @param obj
	 * @return
	 */
	public static boolean loadConfig(File cfgFile, Object obj) {
		Class<?> clazz = obj.getClass();
		
		if (!cfgFile.exists()) {
			return false;
		}
		try (BufferedReader reader = new BufferedReader(new FileReader(cfgFile, StandardCharsets.UTF_8))){
			String line;
			while ((line = reader.readLine()) != null) {
				setAttrByReflection(obj, clazz, line); 
			}
		} catch (IOException | SecurityException e) {
			logger.log(Level.SEVERE, "No se ha podido leer el fichero de configuracion: {}", e.getMessage());
			return false;
		}
		logger.log(Level.FINE, "Clase {} de configuracion iniciada", clazz.getName());
		return true;
	}
	
	/**
	 * 
	 * @param obj
	 * @param clazz
	 * @param line
	 */
	private static void setAttrByReflection(Object obj, Class<?> clazz, String line) {
		if (line.trim().startsWith("#"))
			return;
		String[] lineArr  = line.split("=");
		
		try {
			if (lineArr[0].trim().isEmpty()) {
				return;
			}
			
			Field f = clazz.getDeclaredField(lineArr[0].trim());
			f.setAccessible(true);
		    if (lineArr.length > 1) {
		    	if (f.getType().isAssignableFrom(boolean.class)) {
					f.set(obj, "yes".equals(lineArr[1].trim()) ? true : false);
				} else if (f.getType().isAssignableFrom(String.class)) {
					f.set(obj, lineArr[1].trim());
				} else if (f.getType().isAssignableFrom(List.class)) {
					f.set(obj, Arrays.stream(lineArr[1].trim().split(" "))
					        .collect(Collectors.toList()));
				}
		    }
		} catch (NoSuchFieldException | IllegalAccessException e) {
			logger.log(Level.INFO, String.format("No se ha podido asignar la linea %s. %s", line, e));
		}
	}
	
	/**
	 * 
	 * @param cfgFile
	 * @param obj
	 * @return
	 */
	public static boolean saveConfig(File cfgFile, Object obj,  Map<String, TabElementEmuCfg> hashTabElements) {
		boolean ret = false;
		boolean fileCreated = true;
		ResourceBundle rb = Utils.getDefaultOrFirstResourceBundle();
		
		File fileDir = new File(cfgFile.getParent());
		if (!fileDir.exists() && !fileDir.mkdirs()) {
			JOptionPane.showMessageDialog(null, String.format(rb.getString("alert.error.dirnotcreated"), cfgFile.getName()));
			return false;
		}
		
		try {
			if (!cfgFile.exists()) {
				fileCreated = cfgFile.createNewFile();
			}
		} catch (IOException e) {
			JOptionPane.showMessageDialog(null, String.format(rb.getString("alert.error.cfgdir"), cfgFile.getName()));
			return false;
		}
		
		if (fileCreated) {
			try (Writer writer = new BufferedWriter(new OutputStreamWriter(
			        new FileOutputStream(cfgFile), StandardCharsets.UTF_8))){
				writeAttributesByReflection(obj, writer, hashTabElements);
				ret = true;
				JOptionPane.showMessageDialog(null, String.format(rb.getString("alert.error.cfgsaved"), cfgFile.getAbsolutePath()));
			} catch (IOException e) {
				JOptionPane.showMessageDialog(null, String.format(rb.getString("alert.error.cfgfileerror"), cfgFile.getName()));
			}
		}
		return ret;
	}
	
	/**
	 * 
	 * @param obj
	 * @param writer
	 * @throws IOException
	 */
	private static void writeAttributesByReflection(Object obj, Writer writer, Map<String, TabElementEmuCfg> hashTabElements) throws IOException {
		final String newLine = System.getProperty("line.separator");
		String value;
		String key;
		Class<?> clazz = obj.getClass();
		Field[] fields = clazz.getDeclaredFields();
		for(int i = 0; i < fields.length; i++) {
			key = fields[i].getName();
			try {
				Field f = clazz.getDeclaredField(key);
				f.setAccessible(true);
				
				if (fields[i].getType().isAssignableFrom(boolean.class)) {
					value = (boolean)f.get(obj) ? "yes" : "no";
				} else if (fields[i].getType().isAssignableFrom(String.class)) {
					value = (String)f.get(obj);
				} else {
					value = "";
				}
				
				if (hashTabElements.containsKey(key)) {
					String [] lines = hashTabElements.get(key).getHelp().split("\n");
					for (String line : lines) {
						writer.write(String.format("# %s%n", line));
					}
				}
				writer.write(String.format("%s = %s%s%n", key, value, newLine));
			} catch (NoSuchFieldException e) {
				logger.log(Level.WARNING, String.format("No se ha podido obtener el valor de %s. %s", key, e));
			} catch (SecurityException | IllegalArgumentException | IllegalAccessException e) {
				logger.log(Level.WARNING, String.format("Excepcion de %s. %s", key, e));
			}
		}
	}
	
}
