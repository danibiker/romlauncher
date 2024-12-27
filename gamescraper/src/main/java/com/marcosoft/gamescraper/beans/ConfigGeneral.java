package com.marcosoft.gamescraper.beans;

import java.util.List;

import lombok.AllArgsConstructor;
import lombok.Getter;
import lombok.NoArgsConstructor;
import lombok.Setter;

@Getter
@Setter
@AllArgsConstructor
@NoArgsConstructor
public class ConfigGeneral {
	List<String> emulators;
	boolean debug;
	String resolution;
	String path_prefix;
	boolean convert_enable;
	String convert_prefix_src;
	String convert_prefix_dst;
}
