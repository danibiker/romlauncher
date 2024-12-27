package com.marcosoft.gamescraper.http;

import java.util.HashMap;
import java.util.Map;

import lombok.Data;

@Data
public class TagElement {
	Map<String, String> attrList;
	String content;
	public TagElement() {
		content = "";
		attrList = new HashMap<>();
	}
}

