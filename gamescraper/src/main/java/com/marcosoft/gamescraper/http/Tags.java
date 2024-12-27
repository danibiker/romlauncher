package com.marcosoft.gamescraper.http;

import java.util.ArrayList;
import java.util.List;

import com.marcosoft.gamescraper.http.HtmlParser.HTML_TAG;

import lombok.Data;

@Data
public class Tags {
	HTML_TAG htmlTag;
	List<TagElement> elements;
	public Tags() {
		htmlTag = HTML_TAG.TAG_EMPTY;
		elements = new ArrayList<>();
	}
}