package com.marcosoft.gamescraper.utils;

import java.io.File;
import java.util.ArrayList;
import java.util.Comparator;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import lombok.Getter;
import lombok.Setter;

public class DosNames {
	String allowedChars = "$%'`-@{}~!#()&_^";

	Map<String, String> hashToReplace = new HashMap<>() {
		private static final long serialVersionUID = 1504460359215851058L;
		{
			put(",", "_");
		}
	};

	String regexBrackets2 = "\\[.{0,2}\\]";
	String regexBrackets = "\\[[^]]*\\]";
	String regexParentesis = "\\([^)]*\\)";

	@Getter
	@Setter
	public class FileName8_3 {
		String shortFN;
		String longFN;

		public FileName8_3(String shortFN, String longFN) {
			this.shortFN = shortFN;
			this.longFN = longFN;
		}
	}
	
	/**
	 * 
	 * @param dir
	 * @return
	 */
	public List<FileName8_3> convertirNombresCortos(String dir, List<String> filter) {
		File directorio = new File(dir);
		File[] archivos = directorio.listFiles();
		List<FileName8_3> listFiles = new ArrayList<>();
		
		if (archivos != null) {
			Map<String, Integer> nombresCortos = new HashMap<>();
			for (File archivo : archivos) {
				String extension = (archivo.getName().contains(".") ? archivo.getName().substring(archivo.getName().lastIndexOf('.') + 1) : "").toLowerCase();
				if (archivo.isFile() && (filter.isEmpty() || filter.contains(extension))) {
					String nombreCorto = convertirAFormato83(archivo.getName(), nombresCortos);
					listFiles.add(new FileName8_3(nombreCorto, convertirDescripcion(archivo.getName())));
				}
			}
			listFiles.sort(Comparator.comparing(FileName8_3::getShortFN));
		}
		return listFiles;
	}
	
	/**
	 * 
	 * @param input
	 * @return
	 */
	public String convertirDescripcion(String input) {
		return input.replaceAll(regexBrackets, "").replaceAll(regexParentesis, "");
	}

	/**
	 * 
	 * @param target
	 * @param specialChars
	 * @return
	 */
	public boolean hasSpecialAllowedChars(String target, String specialChars) {
		for (char ch : specialChars.toCharArray()) {
			if (target.indexOf(ch) >= 0) {
				return true; // Found a special character in the target string
			}
		}
		return false; // No special character found
	}

	/**
	 * 
	 * @param nombre
	 * @param nombresCortos
	 * @return
	 */
	public String convertirAFormato83(String nombre, Map<String, Integer> nombresCortos) {
		String nombreFinal = "";
		int counter = 1;
		int nDigits = 1;

		String extension = nombre.contains(".") ? "." + nombre.substring(nombre.lastIndexOf('.') + 1) : "";
		String fileNoExt = Utils.getFileNameWithoutExtension(nombre);
		String nonascci = stripNonAsciiCharacters(fileNoExt);
		String strippedFilename = stripDots(nonascci);

		if (fileNoExt.length() <= 8 && !hasSpecialAllowedChars(fileNoExt, allowedChars)) {
			return (nonascci + extension).toUpperCase();
		}

		String shortfn = first6Characters(strippedFilename, stripNonAsciiCharacters(nombre));
		while (nombresCortos.containsKey((shortfn + "~" + counter))) {
			counter++;
			// if more digits are added to counter, shorten shortfn by 1
			// e.g. if counter comes to 9 and shortf~9.txt is taken. try short~10.txt next
			if (nDigits != String.valueOf(counter).length() && shortfn.length() > 1) {
				nDigits = String.valueOf(counter).length();
				shortfn = shortfn.substring(0, shortfn.length() - 1);
			}
		}
		nombreFinal = (shortfn + "~" + counter + extension.toUpperCase());
		nombresCortos.put((shortfn + "~" + counter), counter);
		
		return nombreFinal;
	}
	
	/**
	 * 
	 * @param dirpath
	 * @return
	 */
    public String convertPathForDos(String dirpath) {
    	String[] dirArr = dirpath.split(File.separator);
    	StringBuilder dirpart = new StringBuilder("");
    	
    	for (int i=0; i < dirArr.length; i++) {
    		if (!dirArr[i].isEmpty()) {
    			dirpart.append(File.separator);
        		Map<String, Integer> nombresCortos = new HashMap<>();
        		dirpart.append(convertirAFormato83(dirArr[i], nombresCortos));
    		}
    	}
    	return dirpart.toString();
    }

	/**
	 * 
	 * @param input
	 * @return
	 */
	private String stripNonAsciiCharacters(String input) {
		StringBuilder output = new StringBuilder();
		for (char c : input.toCharArray()) {
			if ((c <= 127 || allowedChars.indexOf(c) >= 0 || (c >= 0x00A1)) && c != ' ') {
				String tmpC = c >= 0x00A1 ? Character.toString(c) : Character.toString(c).toUpperCase();

				if (hashToReplace.containsKey(tmpC)) {
					output.append(hashToReplace.get(tmpC));
				} else {
					output.append(tmpC);
				}
			}
		}
		return output.toString();
	}

	/**
	 * 
	 * @param input
	 * @return
	 */
	private String stripDots(String input) {
		return input.replace(".", "");
	}
	
	/**
	 * 
	 * @param input
	 * @param nonascii
	 * @return
	 */
	String first6Characters(String input, String nonascii) {
		if (nonascii.lastIndexOf(".") != nonascii.indexOf(".") && nonascii.indexOf(".") > 0
				&& nonascii.indexOf(".") < 6) {
			return stripDots(nonascii.substring(0, nonascii.indexOf(".")));
		}
		
		if (input.length() > 6)
			return input.substring(0, 6);
		else 
			return input;
	}
}
