package com.marcosoft.gamescraper.beans;

import lombok.Getter;
import lombok.Setter;

@Getter
@Setter
public class TabElementEmuCfg {
	
	public TabElementEmuCfg(String label, String help, boolean fileSelector) {
		super();
		this.label = label;
		this.help = help;
		this.fileSelector = fileSelector;
		this.visible = true;
		this.listSelector = false;
		this.mapBtn = false;
	}
	
	String label;
	String help;
	boolean fileSelector;
	boolean visible;
	boolean listSelector;
	boolean mapBtn;
}
