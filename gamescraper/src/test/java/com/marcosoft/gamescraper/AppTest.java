package com.marcosoft.gamescraper;

import java.util.HashMap;
import java.util.Map;

import com.marcosoft.gamescraper.utils.DosNames;

import junit.framework.Test;
import junit.framework.TestCase;
import junit.framework.TestSuite;

/**
 * Unit test for simple App.
 */
public class AppTest 
    extends TestCase
{
    /**
     * Create the test case
     *
     * @param testName name of the test case
     */
    public AppTest( String testName )
    {
        super( testName );
    }

    /**
     * @return the suite of tests being tested
     */
    public static Test suite()
    {
        return new TestSuite( AppTest.class );
    }
    
    public void testFileWithPoints() {
    	String filename = "B.O.B (UEJ) (REV 01) [!].bin";
    	Map<String, Integer> nombresCortos = new HashMap<>();
    	DosNames dos = new DosNames();
    	String corto = dos.convertirAFormato83(filename, nombresCortos);
    	assertEquals("B~1.BIN", corto);
    }
    
    public void testFileWithComma() {
    	String filename = "Ooze, The (5).bin";
    	Map<String, Integer> nombresCortos = new HashMap<>();
    	DosNames dos = new DosNames();
    	String corto = dos.convertirAFormato83(filename, nombresCortos);
    	assertEquals("OOZE_T~1.BIN", corto);
    }
    
    public void testFileLength8() {
    	String filename = "gamelist.xml";
    	Map<String, Integer> nombresCortos = new HashMap<>();
    	DosNames dos = new DosNames();
    	String corto = dos.convertirAFormato83(filename, nombresCortos);
    	assertEquals("GAMELIST.XML", corto);
    }
    
    public void testFileLengthAndSpace() {
    	String filename = "Curse (J).bin";
    	Map<String, Integer> nombresCortos = new HashMap<>();
    	DosNames dos = new DosNames();
    	String corto = dos.convertirAFormato83(filename, nombresCortos);
    	assertEquals("CURSE(~1.BIN", corto);
    }
    
    public void testFileLength8AndSpecialAndSpace() {
    	String filename = "Zool (E).bin";
    	Map<String, Integer> nombresCortos = new HashMap<>();
    	DosNames dos = new DosNames();
    	String corto = dos.convertirAFormato83(filename, nombresCortos);
    	assertEquals("ZOOL(E~1.BIN", corto);
    }
    
    public void test10Elems() {
    	String filename = "shortfieldexample.bin";
    	Map<String, Integer> nombresCortos = new HashMap<>();
    	nombresCortos.put("SHORTF~1", 1);
    	nombresCortos.put("SHORTF~2", 2);
    	nombresCortos.put("SHORTF~3", 3);
    	nombresCortos.put("SHORTF~4", 4);
    	nombresCortos.put("SHORTF~5", 5);
    	nombresCortos.put("SHORTF~6", 6);
    	nombresCortos.put("SHORTF~7", 7);
    	nombresCortos.put("SHORTF~8", 8);
    	nombresCortos.put("SHORTF~9", 9);
    	
    	DosNames dos = new DosNames();
    	String corto = dos.convertirAFormato83(filename, nombresCortos);
    	assertEquals("SHORT~10.BIN", corto);
    }
    
    public void testFileSpecial() {
    	String filename = "Andrè Agassi Tennis.bin";
    	Map<String, Integer> nombresCortos = new HashMap<>();
    	DosNames dos = new DosNames();
    	String corto = dos.convertirAFormato83(filename, nombresCortos);
    	assertEquals("ANDRèA~1.BIN", corto);
    }
}
