package com.marcosoft.gamescraper.http;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import org.jsoup.Jsoup;
import org.jsoup.nodes.Attribute;
import org.jsoup.nodes.Document;
import org.jsoup.nodes.Element;
import org.jsoup.select.Elements;

import com.marcosoft.gamescraper.utils.Utils;

public class HtmlParser {
	List<String> listAttrFound;
	Map<String, String> mapKeyValueFound;
	boolean obtainContentTag;

	public enum HTML_TAG {
		TAG_EMPTY(""), TAG_A("a"), TAG_DIV("div"), TAG_SPAN("span"), TAG_IMG("img"), TAG_SOURCE("source");

		private final String text;

		HTML_TAG(String text) {
			this.text = text;
		}

		public String getText() {
			return text;
		}
	}

	public HtmlParser() {
		listAttrFound = new ArrayList<>();
		obtainContentTag = false;
	}
	
	/**
	 * 
	 * @param html
	 * @param tag1
	 * @param atributoElem1
	 * @param valAtributoElem1
	 * @param tag2
	 * @param atributoElem2
	 * @return
	 */
	public int buscarElemConPadre(String html, HTML_TAG tag1, String atributo, String valAtributo, HTML_TAG tag2,
			String atributoElem2) {
		listAttrFound.clear();
		Document doc = Jsoup.parse(html);
		Elements elems = doc.select(tag1.getText() + "[" + atributo + "*='" + valAtributo + "'] > "
				+ tag2.getText() + "[" + atributoElem2 + "]");
		elems.forEach(el -> listAttrFound.add(el.text()));
		doc.clearAttributes();
		doc.empty();
		elems.clear();
		return listAttrFound.size();
	}
	
	/**
	 * 
	 * @param html
	 * @param tag1
	 * @param atributo
	 * @param valAtributo
	 * @return
	 */
	public Tags buscarElemento(String html, HTML_TAG tag, String atributo, String valAtributo) {
		listAttrFound.clear();
		Document doc = Jsoup.parse(html);
		Elements elems = doc.select(tag.getText() + (!Utils.isEmpty(atributo) ? "[" + atributo + "*='" + valAtributo + "']" : ""));
		Tags tags = new Tags();
		tags.setHtmlTag(tag);
		for (Element elem : elems) {
			TagElement tagElement = new TagElement();
			if (isObtainContentTag()) tagElement.setContent(elem.text());
			
			for (Attribute attr : elem.attributes()) {
				tagElement.getAttrList().put(attr.getKey(), attr.getValue());
			}
			tags.getElements().add(tagElement);
		}
		doc.clearAttributes();
		doc.empty();
		elems.clear();
		return tags;
	}

	/**
	 * 
	 * @param html
	 * @param tag
	 * @return
	 */
	public Tags buscarElementos(String html, HTML_TAG tag) {
		Document doc = Jsoup.parse(html);
		Elements elems = doc.select(tag.getText());
		Tags tags = new Tags();
		tags.setHtmlTag(tag);
		for (Element elem : elems) {
			TagElement tagElement = new TagElement();
			if (isObtainContentTag()) tagElement.setContent(elem.text());
			
			for (Attribute attr : elem.attributes()) {
				tagElement.getAttrList().put(attr.getKey(), attr.getValue());
			}
			tags.getElements().add(tagElement);
		}
		doc.clearAttributes();
		doc.empty();
		elems.clear();
		return tags;
	}

	public List<String> getListAttrFound() {
		return listAttrFound;
	}

	public Map<String, String> getMapKeyValueFound() {
		return mapKeyValueFound;
	}

	public boolean isObtainContentTag() {
		return obtainContentTag;
	}

	public void setObtainContentTag(boolean obtainContentTag) {
		this.obtainContentTag = obtainContentTag;
	}

}
