package com.marcosoft.gamescraper;

import java.awt.BorderLayout;
import java.awt.CardLayout;
import java.awt.Color;
import java.awt.Component;
import java.awt.Dimension;
import java.awt.EventQueue;
import java.awt.Font;
import java.awt.HeadlessException;
import java.awt.Image;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.ComponentAdapter;
import java.awt.event.ComponentEvent;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.awt.image.BufferedImage;
import java.awt.image.ColorModel;
import java.awt.image.IndexColorModel;
import java.awt.image.WritableRaster;
import java.beans.PropertyVetoException;
import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.ByteArrayInputStream;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStreamWriter;
import java.io.Writer;
import java.lang.reflect.Field;
import java.net.URI;
import java.net.URISyntaxException;
import java.net.URL;
import java.nio.charset.StandardCharsets;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.Comparator;
import java.util.HashMap;
import java.util.List;
import java.util.Locale;
import java.util.Map;
import java.util.MissingResourceException;
import java.util.ResourceBundle;
import java.util.concurrent.Executors;
import java.util.concurrent.ThreadPoolExecutor;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.logging.Handler;
import java.util.logging.Level;
import java.util.logging.LogManager;
import java.util.logging.Logger;
import java.util.stream.Collectors;

import javax.imageio.ImageIO;
import javax.swing.DefaultComboBoxModel;
import javax.swing.DefaultListModel;
import javax.swing.Icon;
import javax.swing.ImageIcon;
import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JComboBox;
import javax.swing.JFileChooser;
import javax.swing.JFrame;
import javax.swing.JInternalFrame;
import javax.swing.JLabel;
import javax.swing.JLayeredPane;
import javax.swing.JList;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTabbedPane;
import javax.swing.JTextArea;
import javax.swing.JTextField;
import javax.swing.ListSelectionModel;
import javax.swing.SwingConstants;
import javax.swing.UIManager;
import javax.swing.WindowConstants;
import javax.swing.border.BevelBorder;
import javax.swing.border.LineBorder;
import javax.swing.border.TitledBorder;
import javax.swing.filechooser.FileNameExtensionFilter;
import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;

import org.apache.hc.core5.net.URIBuilder;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;
import org.xml.sax.SAXException;

import com.marcosoft.gamescraper.beans.ConfigEmu;
import com.marcosoft.gamescraper.beans.ConfigGeneral;
import com.marcosoft.gamescraper.beans.TabElementEmuCfg;
import com.marcosoft.gamescraper.http.HttpUtil;
import com.marcosoft.gamescraper.utils.CfgFile;
import com.marcosoft.gamescraper.utils.DosNames;
import com.marcosoft.gamescraper.utils.Utils;

import jiconfont.icons.font_awesome.FontAwesome;
import jiconfont.swing.IconFontSwing;
import lombok.AllArgsConstructor;
import lombok.Getter;
import lombok.NoArgsConstructor;
import lombok.Setter;
import net.coobird.thumbnailator.Thumbnails;

/**
 * Hello world!
 *
 */
public class App 
{
	private static final String LINE_SEPARATOR = "line.separator";
	private static final String DIR_SYNOPSIS = "synopsis";
	private static final String DIR_BOX2D = "box2d";
	private static final String DIR_SNAPTIT = "snaptit";
	private static final String SS = "ss";
	private static final String SSTITLE = "sstitle";
	private static final String FORMAT = "format";
	private static final String TYPE = "type";
	private static final String BOX_2D = "box-2D";
	private static final String REGION = "region";
	private static final String LANGUE = "langue";
	private static final String SYNOPSIS = DIR_SYNOPSIS;
	private static final String MEDIAS = "medias";
	private static final String DIR_SNAP = "snap";
	private static final String MAME_DAT = "MAME.DAT";
	private static final Integer MAME_SYS_ID = 75;
	private static final String ROM_EXTENSION = "rom_extension";
	private static final String USE_EXTENSION = "use_extension";
	private static final String USE_ROM_DIRECTORY = "use_rom_directory";
	private static final String SYSTEM = "system";
	private static final String ASSETS = "assets";
	private static final String GMENU_CFG = "gmenu.cfg";
	private static final String CONFIG = "config";
	private static final String GMENU = "gmenu";
	private static final String MAP_FILE = "map_file";
	private static final String ROM_DIRECTORY = "rom_directory";
	private static final String DIRECTORY = "directory";
	private static final String SCREEN_SHOT_DIRECTORY = "screen_shot_directory";
	private static final String EXECUTABLE = "executable";
	private JFrame frame;
	private JMenuBar menuBar;
	private JPanel pnlScrapper;
	private JCheckBox chkIgnore;
	JTabbedPane tabsMain;
	JButton btnBuscar;
	JPanel pnlGeneral;
	JPanel pnlMain;
	JCheckBox chkDebug;
	JCheckBox chkConvert;
	Icon iconSave;
	JInternalFrame windowScrapper;
	JLabel lblDownTotalRoms;
	JLabel lblDescargandoRecurso;
	JLabel lblDownSystem;
	Icon iconMsDos;
	Icon iconSelectAll;
	Icon iconSelectNone;
	ThSearchSystems thSearch;
	JComboBox<ImgConversion> comboImgConv;
	JComboBox<ScrapeLang> cmbLang;
	JCheckBox chkMediaSize;
	JLabel lblDownProgress;
	//private final Map<Integer, IndexColorModel> colorModel = new HashMap<>();
	ResourceBundle rb;
	
	private final Logger logger = Logger.getLogger(App.class.getName());
	private static final String SCREENSCRAPERAPIURL = "https://api.screenscraper.fr/api2/";
	private Map<String, String> fixedParms;
	private List<String> imagesToFind = Arrays.asList(new String[] { SSTITLE, SS, BOX_2D });
	private List<String> synopsisLangToFind = new ArrayList<>();
	private List<String> regionToFind = Arrays.asList(new String[] { "eu", "us", "jp", "asi", "au", "br", "kr", SS});
	
	private JLayeredPane layeredPane;
	private JTextField txtPrefix;
	private Map<String, ConfigEmu> mapEmus;
	private ConfigGeneral cfgGeneral = new ConfigGeneral();
	private int iMediasDownloaded = 0;
	
	Map<String, TabElementEmuCfg> hashTabElements;
	private JTextField txtH;
	private JTextField txtW;
	private JPanel panel_1;
	private JTextField txtDirReplaced;
	private JTextField txtDirToReplace;
	private JLabel label;
	private JList<ScrapSystem> listSystemsToScrap;
	private JTextField txtMediaW;
	private JTextField txtMediaH;
	
	//Declare list as an enum
    private enum WEBS_SCRAPE
    {
        SCREENSCRAPPER ("ScreenScrapper");

        private final String name;
        private WEBS_SCRAPE(String name)
        {
           this.name = name;
        }
        @Override
        public String toString()
        {
            return name;
        }
    }
    
    @Getter
    private enum IMG_CONVERSIONS
    {
        NO_CONVERSION (-1), 
        PNG_INDEXED(0);
    	
        private final int value;
        
        private IMG_CONVERSIONS(int value)
        {
           this.value = value;
        }
    }
    
    
    
    @Getter
    @Setter
    private class Rom{
    	String longFileName;
    	String shortFileName;
    	String image;
    	
    	public Rom() {
    		
    	}
    }
    
    @Getter
    @Setter
    @AllArgsConstructor
    @NoArgsConstructor
    private class GameSystem{
    	Integer id;
    	String name;
    	
    	 public String toString() {
		    return getName();
		 }
    }
    
    @Getter
    @Setter
    @AllArgsConstructor
    @NoArgsConstructor
    private class ScrapeLang{
    	String id;
    	String desc;
    	
    	 public String toString() {
		    return getDesc();
		 }
    }
    
    @Getter
    @Setter
    @AllArgsConstructor
    @NoArgsConstructor
    private class ImgConversion{
    	int id;
    	String name;
    	
    	 public String toString() {
		    return getName();
		 }
    }
    
    @Getter
    @Setter
    @AllArgsConstructor
    @NoArgsConstructor
    private class ScrapSystem{
    	Integer id;
    	String name;
    	
    	 public String toString() {
		    return getName();
		 }
    }

    @Getter
    @Setter
    private class GameImage{
    	String url;
    	String filetype;
    }
    
    @Getter
    @Setter
    private class Synopsis{
    	String lang;
    	String text;
    }
    
    @Getter
    @Setter
    private class GameInfo{
    	HashMap<String, GameImage> images;
    	HashMap<String, Synopsis> synopsis;
    }
	
    /**
     * 
     * @param args
     */
    public static void main( String[] args )
    {
    	EventQueue.invokeLater(new Runnable() {
			public void run() {
				try {
					App app = new App();
					app.setDebugLevel(Level.INFO);
				} catch (Exception e) {
					e.printStackTrace();
				}
			}
		});
    }
    
	/**
	 * Define the default level of logs. Each level only showing the down ones
	 * 
	 * FINEST -> Even more detailed info of operations inside methods
	 * FINER  -> Detailed info of operations inside methods
	 * FINE   -> Entering or exiting methods 
	 * CONFIG -> Events messages or important information
	 * INFO   -> Potential problems like connections refused, timeouts, etc.
	 * WARNING-> Unexpected exceptions not catastrophic
	 * SEVERE -> Serious failure preventing normal execution like out of memory errors 
	 * 
	 * @param newLvl
	 */
	public void setDebugLevel(Level newLvl) {
	    Logger rootLogger = LogManager.getLogManager().getLogger("");
	    rootLogger.setLevel(newLvl);
	    for (Handler h : rootLogger.getHandlers()) {
	        h.setLevel(newLvl);
	    }
	}
	
	/**
	 * 
	 * @param pathdir
	 * @return
	 */
	public int loadCfgEmus(String pathdir) {
		txtPrefix.setText(pathdir);
		File dirCfg = new File(pathdir + File.separator + GMENU + File.separator + CONFIG);
		File dirGmenu = new File(pathdir + File.separator + GMENU);
		File fileCfg = new File(txtPrefix.getText() + File.separator + GMENU + File.separator + GMENU_CFG);
		
		if (!dirGmenu.exists() || !dirCfg.exists()) {
			return 1;
		} 
		
		if(dirGmenu.isDirectory() && CfgFile.loadConfig(fileCfg, cfgGeneral)) {
			//Avoid writing the prefix with the data obtained by the config file. 
			//It will be written ok when save and the chkConvert is selected
			//txtPrefix.setText(cfgGeneral.getPath_prefix());
			//Instead, write to txtDirReplaced
			txtDirReplaced.setText(txtPrefix.getText());
			String [] res = cfgGeneral.getResolution().trim().split(" ");
			if (res.length > 0) txtW.setText(res[0]);
			if (res.length > 1) txtH.setText(res[1]);
			chkDebug.setSelected(cfgGeneral.isDebug());
			chkConvert.setSelected(cfgGeneral.isConvert_enable());
			txtDirToReplace.setText(cfgGeneral.getConvert_prefix_dst());
			txtDirReplaced.setEnabled(chkConvert.isSelected());
			txtDirToReplace.setEnabled(chkConvert.isSelected());
			
			if (dirCfg.exists() && dirCfg.isDirectory()) {
				for (File file : dirCfg.listFiles(file -> file.getName().toLowerCase().endsWith(".cfg"))) {
					ConfigEmu configEmu = new ConfigEmu();
					CfgFile.loadConfig(file, configEmu);
					String idemu = Utils.getFileNameWithoutExtension(file.getName());
					mapEmus.put(idemu, configEmu);
					addTabEmu(idemu);
				}
			}
		} else {
			return 2;
		}
		return 0;
	}
	
	
    /**
     * 
     */
    public App() {
    	IconFontSwing.register(FontAwesome.getIconFont());
    	mapEmus = new HashMap<>();
    	findTextResources();
            	
    	fixedParms = new HashMap<>();
    	fixedParms.put("devid", "jelos");
    	fixedParms.put("devpassword", "jelos");
    	fixedParms.put("softname", "scrapdos1");
    	fixedParms.put("output", "xml");
    	fixedParms.put("ssid", "test11");
    	fixedParms.put("sspassword", "test11");
    	
    	frame = new JFrame();
		frame.setVisible(true);
		frame.setBounds(100, 100, 800, 600);
		frame.setLocationRelativeTo(null); // center the frame on screen
		frame.setDefaultCloseOperation(WindowConstants.EXIT_ON_CLOSE);
		
		menuBar = new JMenuBar();
		frame.setJMenuBar(menuBar);
		
		JMenu menuArchivo = new JMenu(rb.getString("menu.archive"));
		menuBar.add(menuArchivo);
		JMenuItem mntmExit = new JMenuItem(rb.getString("menu.close"));
		mntmExit.addActionListener(e -> System.exit(0));
		menuArchivo.add(mntmExit);
		
		JMenu menuMedia = new JMenu(rb.getString("menu.medias"));
		menuBar.add(menuMedia);
		
		JMenuItem mntmScrapper = new JMenuItem(rb.getString("menu.scrapper"));
		mntmScrapper.addActionListener(e -> {
			int x = (frame.getWidth() - windowScrapper.getWidth()) / 2;
			int y = (frame.getHeight() - windowScrapper.getHeight()) / 2;
			windowScrapper.setBounds(x, y, windowScrapper.getBounds().width, windowScrapper.getBounds().height);
			
			findScrappableSystems();
			windowScrapper.setVisible(true);
		});
		menuMedia.add(mntmScrapper);
		frame.getContentPane().setLayout(new BorderLayout(0, 0));
		
		layeredPane = new JLayeredPane();
		frame.getContentPane().add(layeredPane);
		layeredPane.addComponentListener(new ResizeListener());
		
		pnlMain = new JPanel();
		pnlMain.setBounds(0, 0, 319, 461);
		pnlMain.setLayout(new CardLayout(0, 0));
		
		tabsMain = new JTabbedPane(SwingConstants.TOP);
		tabsMain.setTabLayoutPolicy(JTabbedPane.SCROLL_TAB_LAYOUT);
		pnlMain.add(tabsMain, "name_336428625846");
		
		// Add panels with custom titles and icons
		pnlGeneral = new JPanel();
		tabsMain.addTab(rb.getString("tabs.general"), null, pnlGeneral, rb.getString("tabs.general.tip"));
		pnlGeneral.setLayout(null);
		
		JLabel lblNombreDelEmulador = new JLabel(rb.getString("lbl.addemu"));
		lblNombreDelEmulador.setBounds(12, 34, 127, 15);
		pnlGeneral.add(lblNombreDelEmulador);
		
		JLabel lblDirectorioPrefijo = new JLabel(rb.getString("lbl.prefix"));
		lblDirectorioPrefijo.setBounds(12, 61, 165, 15);
		pnlGeneral.add(lblDirectorioPrefijo);
		
		txtPrefix = new JTextField();
		txtPrefix.setText("/home/daniel/Apps/Juegos/dosbox/doshd/juegos/emu2");
		txtPrefix.setBounds(12, 85, 415, 25);
		pnlGeneral.add(txtPrefix);
		txtPrefix.setColumns(10);
		
		JButton btnPrefix = new JButton("...");
		btnPrefix.addActionListener(actionEvent -> {
				JFileChooser chooser = new JFileChooser();
				chooser.setFileSelectionMode(JFileChooser.DIRECTORIES_ONLY);
				chooser.setAcceptAllFileFilterUsed(true);
								
				int returnVal = chooser.showOpenDialog(tabsMain);
				if(returnVal == JFileChooser.APPROVE_OPTION) {
					int emusLoadRet = loadCfgEmus(chooser.getSelectedFile().getAbsolutePath());
					if (emusLoadRet == 1) {
						int result = JOptionPane.showConfirmDialog(frame, rb.getString("alert.createdir"), rb.getString("alert.title"),
					               JOptionPane.YES_NO_OPTION,
					               JOptionPane.QUESTION_MESSAGE);
						
			            if(result == JOptionPane.YES_OPTION){
			            	if (!new File(chooser.getSelectedFile().getAbsolutePath() 
			            			+ File.separator + GMENU + File.separator + CONFIG).mkdirs()) {
			            		JOptionPane.showMessageDialog(frame, rb.getString("alert.error.dircreation"));
			            		return;
			            	}
			            } 
					} 
					
					if (emusLoadRet == 2) {
						File dirGmenu = new File(chooser.getSelectedFile().getAbsolutePath() + File.separator + GMENU);
						File fileCfg = new File(txtPrefix.getText() + File.separator + GMENU + File.separator + GMENU_CFG);
						if (dirGmenu.isDirectory()) {
							JOptionPane.showMessageDialog(frame, rb.getString("alert.error.filenotfound") + fileCfg);
						} else {
							JOptionPane.showMessageDialog(frame, rb.getString("alert.error.dirnotselected") + dirGmenu.getAbsolutePath());
						}
					}
				}
		});
		
		btnPrefix.setBounds(439, 85, 25, 25);
		pnlGeneral.add(btnPrefix);
		
		JButton btnAddEmu = new JButton();
		btnAddEmu.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent arg0) {
				addEmu();
			}
		});
		btnAddEmu.setBounds(139, 29, 25, 25);
		// Register the IconFont
        Icon icon = IconFontSwing.buildIcon(FontAwesome.PLUS, 10, new Color(0, 0, 0));
        btnAddEmu.setIcon(icon);
		pnlGeneral.add(btnAddEmu);

		chkDebug = new JCheckBox(rb.getString("lbl.debug"));
		chkDebug.setBounds(147, 122, 129, 23);
		pnlGeneral.add(chkDebug);
		
		JPanel panel = new JPanel();
		panel.setBorder(new TitledBorder(null, rb.getString("lbl.resolution"), TitledBorder.LEADING, TitledBorder.TOP, null, null));
		panel.setBounds(12, 122, 127, 88);
		pnlGeneral.add(panel);
		panel.setLayout(null);
		
		JLabel lblAncho = new JLabel(rb.getString("res.width"));
		lblAncho.setBounds(12, 24, 43, 15);
		panel.add(lblAncho);
		
		JLabel lblAlto = new JLabel(rb.getString("res.height"));
		lblAlto.setBounds(70, 24, 57, 15);
		panel.add(lblAlto);
		
		txtH = new JTextField();
		txtH.setText("240");
		txtH.setHorizontalAlignment(SwingConstants.CENTER);
		txtH.setColumns(10);
		txtH.setBounds(70, 44, 43, 19);
		panel.add(txtH);
		
		JLabel lblX = new JLabel("x");
		lblX.setBounds(58, 46, 7, 15);
		panel.add(lblX);
		
		txtW = new JTextField();
		txtW.setText("320");
		txtW.setHorizontalAlignment(SwingConstants.CENTER);
		txtW.setColumns(10);
		txtW.setBounds(12, 44, 43, 19);
		panel.add(txtW);
		iconSave = IconFontSwing.buildIcon(FontAwesome.FLOPPY_O, 20, new Color(0, 0, 0));
		
		panel_1 = new JPanel();
		panel_1.setBorder(new TitledBorder(null, rb.getString("lbl.saveconvert"), TitledBorder.LEADING, TitledBorder.TOP, null, null));
		panel_1.setBounds(12, 222, 771, 170);
		pnlGeneral.add(panel_1);
		panel_1.setLayout(null);
		
		JButton btnSave = new JButton("");
		btnSave.setBounds(12, 116, 25, 25);
		panel_1.add(btnSave);
		btnSave.addActionListener(arg0 -> {
			createConfig();
		});
		btnSave.setToolTipText(rb.getString("lbl.savechanges"));
		btnSave.setIcon(iconSave);
		
		txtDirReplaced = new JTextField();
		txtDirReplaced.setEnabled(false);
		txtDirReplaced.setColumns(10);
		txtDirReplaced.setBounds(12, 79, 349, 25);
		panel_1.add(txtDirReplaced);
		
		txtDirToReplace = new JTextField();
		txtDirToReplace.setEnabled(false);
		txtDirToReplace.setColumns(10);
		txtDirToReplace.setBounds(410, 79, 349, 25);
		panel_1.add(txtDirToReplace);
		
		JTextArea txtrReemplazarLasRutas = new JTextArea();
		txtrReemplazarLasRutas.addMouseListener(new MouseAdapter() {
			@Override
			public void mouseClicked(MouseEvent e) {
				chkConvert.doClick();
			}
		});
		txtrReemplazarLasRutas.setWrapStyleWord(true);
		txtrReemplazarLasRutas.setFont(new Font("Dialog", Font.BOLD, 12));
		txtrReemplazarLasRutas.setForeground(Color.BLACK);
		txtrReemplazarLasRutas.setLineWrap(true);
		txtrReemplazarLasRutas.setOpaque(false);
		txtrReemplazarLasRutas.setText(rb.getString("lbl.saveconvert.tip"));
		txtrReemplazarLasRutas.setBounds(45, 22, 714, 45);
		panel_1.add(txtrReemplazarLasRutas);
		
		label = new JLabel("->");
		label.setHorizontalAlignment(SwingConstants.CENTER);
		label.setBounds(375, 79, 25, 25);
		panel_1.add(label);
		
		chkConvert = new JCheckBox("");
		chkConvert.addActionListener(e -> {
        	txtDirReplaced.setEnabled(chkConvert.isSelected());
			txtDirToReplace.setEnabled(chkConvert.isSelected());
        });
		chkConvert.setBounds(12, 34, 25, 23);
		panel_1.add(chkConvert);
		
		pnlScrapper = new JPanel();
		pnlScrapper.setVisible(true);
		pnlScrapper.setBorder(new BevelBorder(BevelBorder.LOWERED, null, null, null, null));
		pnlScrapper.setBackground(UIManager.getColor("Panel.background"));
		pnlScrapper.setLayout(null);
		
		frame.setDefaultCloseOperation(JFrame.HIDE_ON_CLOSE);
		windowScrapper = new JInternalFrame(rb.getString("lbl.media.obtain"));
		windowScrapper.setResizable(true);
		windowScrapper.setBounds(424, 12, 362, 431);
		windowScrapper.setClosable(true);
		windowScrapper.getContentPane().add(pnlScrapper, BorderLayout.CENTER);
		windowScrapper.setVisible(false);
		layeredPane.setLayout(null);
		
		JInternalFrame winDownloadStatus = new JInternalFrame(rb.getString("lbl.media.downloading"));
		winDownloadStatus.setBounds(-27, 343, 374, 165);
		layeredPane.add(winDownloadStatus);
		winDownloadStatus.getContentPane().setLayout(new BorderLayout(0, 0));
		
		JPanel panel_3 = new JPanel();
		winDownloadStatus.getContentPane().add(panel_3);
		panel_3.setLayout(null);
		
		
		lblDownProgress = new JLabel(rb.getString("lbl.media.progress"));
		lblDownProgress.setBounds(4, 5, 350, 24);
		panel_3.add(lblDownProgress);
		lblDownProgress.setFont(new Font("Dialog", Font.BOLD, 20));
		lblDownProgress.setHorizontalAlignment(SwingConstants.CENTER);
		
		lblDownSystem = new JLabel("System");
		lblDownSystem.setBounds(4, 30, 350, 15);
		panel_3.add(lblDownSystem);
		lblDownSystem.setHorizontalAlignment(SwingConstants.CENTER);
		
		lblDownTotalRoms = new JLabel("");
		lblDownTotalRoms.setBounds(4, 49, 350, 15);
		panel_3.add(lblDownTotalRoms);
		lblDownTotalRoms.setHorizontalAlignment(SwingConstants.CENTER);
		
		lblDescargandoRecurso = new JLabel(rb.getString("lbl.media.download"));
		lblDescargandoRecurso.setBounds(4, 73, 350, 15);
		panel_3.add(lblDescargandoRecurso);
		lblDescargandoRecurso.setHorizontalAlignment(SwingConstants.CENTER);
		
		JButton btnDownCancel = new JButton(rb.getString("lbl.media.end"));
		btnDownCancel.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent arg0) {
				thSearch.interrupt();
				winDownloadStatus.setVisible(false);
				windowScrapper.setVisible(true);
			}
		});
		btnDownCancel.setBounds(126, 100, 97, 25);
		panel_3.add(btnDownCancel);
		
		layeredPane.add(pnlMain);
		layeredPane.add(windowScrapper);
		
		JComboBox<WEBS_SCRAPE> comboBox = new JComboBox<>(WEBS_SCRAPE.values());
		comboBox.setBounds(12, 31, 150, 24);
		pnlScrapper.add(comboBox);
		
		JCheckBox chkOverwrite = new JCheckBox(rb.getString("lbl.replace"));
		chkOverwrite.setBounds(12, 309, 188, 23);
		pnlScrapper.add(chkOverwrite);
		
		chkIgnore = new JCheckBox(rb.getString("lbl.ignore"));
		chkIgnore.setBounds(12, 336, 233, 23);
		chkIgnore.setSelected(true);
		pnlScrapper.add(chkIgnore);
		
		btnBuscar = new JButton(rb.getString("lbl.search"));
		btnBuscar.setBounds(12, 367, 82, 25);
		btnBuscar.setEnabled(false);
		
		btnBuscar.addActionListener(e -> {
			searchRoms(winDownloadStatus);
		});
		
		pnlScrapper.add(btnBuscar);	
		
		JPanel panel_2 = new JPanel();
		panel_2.setBorder(new TitledBorder(new LineBorder(new Color(184, 207, 229)), rb.getString("lbl.systems"), TitledBorder.LEADING, TitledBorder.TOP, null, new Color(51, 51, 51)));
		panel_2.setBounds(12, 67, 331, 174);
		pnlScrapper.add(panel_2);
		panel_2.setLayout(null);
		
		
		listSystemsToScrap = new JList<>();
		listSystemsToScrap.setBorder(new BevelBorder(BevelBorder.LOWERED, null, null, null, null));
		listSystemsToScrap.setSelectionMode(ListSelectionModel.MULTIPLE_INTERVAL_SELECTION);
		JScrollPane scrollPane = new JScrollPane(listSystemsToScrap);
		scrollPane.setBounds(3, 59, 323, 115);
		panel_2.add(scrollPane);
		
		
		
        iconSelectAll = IconFontSwing.buildIcon(FontAwesome.LIST_UL, 20, new Color(0, 0, 0));
        iconSelectNone = IconFontSwing.buildIcon(FontAwesome.LIST, 20, new Color(128, 128, 128));

		JButton btnSelectAll = new JButton(rb.getString("lbl.all"));
		btnSelectAll.addActionListener(e ->{
			int begn = 0;
			int end = listSystemsToScrap.getModel().getSize() - 1;
			if (end >= 0) {
				listSystemsToScrap.setSelectionInterval(begn, end);
			}
		});
		
		btnSelectAll.setBounds(12, 22, 129, 25);
		btnSelectAll.setIcon(iconSelectAll);
		panel_2.add(btnSelectAll);
		
		JButton btnSelectNone = new JButton(rb.getString("lbl.none"));
		btnSelectNone.addActionListener(e -> {
			listSystemsToScrap.clearSelection();
		});
		btnSelectNone.setBounds(153, 22, 129, 25);
		btnSelectNone.setIcon(iconSelectNone);
		panel_2.add(btnSelectNone);
		
		comboImgConv = new JComboBox<>();
		fillImgConv(comboImgConv);
		comboImgConv.setBounds(12, 277, 170, 24);
		pnlScrapper.add(comboImgConv);
		
		JLabel lblConvertirImgenes = new JLabel(rb.getString("lbl.img.convert"));
		lblConvertirImgenes.setBounds(12, 257, 138, 15);
		pnlScrapper.add(lblConvertirImgenes);
		
		chkMediaSize = new JCheckBox(rb.getString("lbl.img.resize"));
		chkMediaSize.addActionListener(e -> {
			txtMediaW.setEnabled(chkMediaSize.isSelected());
			txtMediaH.setEnabled(chkMediaSize.isSelected());
        });
		chkMediaSize.setBounds(192, 253, 151, 23);
		pnlScrapper.add(chkMediaSize);
		
		txtMediaW = new JTextField();
		txtMediaW.setEnabled(false);
		txtMediaW.setHorizontalAlignment(SwingConstants.CENTER);
		txtMediaW.setText("320");
		txtMediaW.setBounds(194, 280, 48, 19);
		pnlScrapper.add(txtMediaW);
		txtMediaW.setColumns(10);
		
		txtMediaH = new JTextField();
		txtMediaH.setEnabled(false);
		txtMediaH.setHorizontalAlignment(SwingConstants.CENTER);
		txtMediaH.setText("320");
		txtMediaH.setColumns(10);
		txtMediaH.setBounds(254, 280, 48, 19);
		pnlScrapper.add(txtMediaH);
		
		JLabel lblX_1 = new JLabel("x");
		lblX_1.setBounds(245, 282, 13, 15);
		pnlScrapper.add(lblX_1);
		
		cmbLang = new JComboBox<>();
		cmbLang.setBounds(193, 31, 150, 24);
		fillLangConv(cmbLang);
		pnlScrapper.add(cmbLang);
		
		JLabel lblWebDeMedios = new JLabel(rb.getString("lbl.medias.web"));
		lblWebDeMedios.setBounds(12, 15, 109, 15);
		pnlScrapper.add(lblWebDeMedios);
		
		JLabel lblNewLabel = new JLabel(rb.getString("lbl.download.lang"));
		lblNewLabel.setBounds(192, 15, 151, 15);
		pnlScrapper.add(lblNewLabel);
		
		//auto loading the configuration if there is any
		Path cwd = Path.of("").toAbsolutePath();
		int posPrevDir = cwd.toString().indexOf(File.separator + GMENU);
		if (posPrevDir > 0) {
			String parentDir = cwd.toString().substring(0, posPrevDir);
			loadCfgEmus(parentDir);
		}
    }
    

    
    /**
     * 
     */
	private void findTextResources() {
		rb = Utils.getDefaultOrFirstResourceBundle();
    	if (rb != null) {
    		hashTabElements = new HashMap<>() {
        		private static final long serialVersionUID = 1504460359215851058L;
        		{
        			put("name", new TabElementEmuCfg(rb.getString("emu.name"),rb.getString("emu.name.tip"), false));
        			put("description", new TabElementEmuCfg(rb.getString("emu.desc"),rb.getString("emu.desc.tip"), false));
        			put(DIRECTORY, new TabElementEmuCfg(rb.getString("emu.dir"),rb.getString("emu.dir.tip"), true));
        			put(EXECUTABLE, new TabElementEmuCfg(rb.getString("emu.exec"),rb.getString("emu.exec.tip"), true));
        			put("global_options", new TabElementEmuCfg(rb.getString("emu.opt"),rb.getString("emu.opt.tip"), false));
        			put("options_before_rom", new TabElementEmuCfg(rb.getString("emu.optbef"),rb.getString("emu.optbef.tip"), false));
        			put("use_rom_file", new TabElementEmuCfg(rb.getString("emu.chkromfile"),rb.getString("emu.chkromfile.tip"), false));
        			put(MAP_FILE, new TabElementEmuCfg(rb.getString("emu.romfile"),rb.getString("emu.romfile.tip"), true));
        			put(ASSETS, new TabElementEmuCfg(rb.getString("emu.assets"),rb.getString("emu.assets.tip"), true));
        			put(SCREEN_SHOT_DIRECTORY, new TabElementEmuCfg(rb.getString("emu.sshot"),rb.getString("emu.sshot.tip"), true));
        			put(USE_ROM_DIRECTORY, new TabElementEmuCfg(rb.getString("emu.chkromdir"),rb.getString("emu.chkromdir.tip"), false));
        			put(ROM_DIRECTORY, new TabElementEmuCfg(rb.getString("emu.romdir"),rb.getString("emu.romdir.tip"), true));
        			put(ROM_EXTENSION, new TabElementEmuCfg(rb.getString("emu.ext"),rb.getString("emu.ext.tip"), false));
        			put(USE_EXTENSION, new TabElementEmuCfg(rb.getString("emu.chkext"),rb.getString("emu.chkext.tip"), false));
        			put(SYSTEM, new TabElementEmuCfg("Sistema","Sistema emulado", false));
        			
        			this.get(SCREEN_SHOT_DIRECTORY).setVisible(false);
        			this.get(SYSTEM).setListSelector(true);
        			this.get(MAP_FILE).setMapBtn(true);
        		}
        	};
    	}
	}
    
    /**
     * 
     * @param combo
     * @return
     */
    public int fillLangConv(JComboBox<ScrapeLang> combo) {
    	DefaultComboBoxModel<ScrapeLang> comboBoxModel = new DefaultComboBoxModel<>();
    	Locale locale = Locale.getDefault();
    	comboBoxModel.addElement(new ScrapeLang("en", rb.getString("cmb.lang.en")));
		comboBoxModel.addElement(new ScrapeLang("es", rb.getString("cmb.lang.es")));
    	combo.setModel(comboBoxModel);
    	
    	for (int i=0; i < comboBoxModel.getSize(); i++) {
    		if (comboBoxModel.getElementAt(i).getId().equals(locale.getCountry().toLowerCase())) {
    			synopsisLangToFind.addFirst(comboBoxModel.getElementAt(i).getId());
    			comboBoxModel.setSelectedItem(comboBoxModel.getElementAt(i));
    		} else {
    			synopsisLangToFind.add(comboBoxModel.getElementAt(i).getId());
    		}
    	}
    	return comboBoxModel.getSize();
    }
    
    /**
     * 
     * @param combo
     * @return
     */
    public int fillImgConv(JComboBox<ImgConversion> combo) {
    	DefaultComboBoxModel<ImgConversion> comboBoxModel = new DefaultComboBoxModel<>();
		comboBoxModel.addElement(new ImgConversion(IMG_CONVERSIONS.NO_CONVERSION.getValue(), rb.getString("cmb.conv.none")));
		comboBoxModel.addElement(new ImgConversion(IMG_CONVERSIONS.PNG_INDEXED.getValue(), rb.getString("cmb.conv.png")));
    	combo.setModel(comboBoxModel);
    	return comboBoxModel.getSize();
    }

	private void searchRoms(JInternalFrame winDownloadStatus) {
		winDownloadStatus.setVisible(true);
		windowScrapper.setVisible(false);

		try {
			final int x = (frame.getWidth() - winDownloadStatus.getWidth()) / 2;
			final int y = (frame.getHeight() - winDownloadStatus.getHeight()) / 2;
			winDownloadStatus.setBounds(x, y, winDownloadStatus.getBounds().width, winDownloadStatus.getBounds().height);
			winDownloadStatus.setSelected(true);
		} catch (PropertyVetoException e1) {
			logger.log(Level.SEVERE, String.format("window not selectable %s", e1.getMessage()));
		}
		
		//Setting the order of preference for languages to download
		ScrapeLang lang = (ScrapeLang)cmbLang.getSelectedItem();
		int pos = synopsisLangToFind.indexOf(lang.id);
		if (pos > 0) {
			Collections.rotate(synopsisLangToFind, -pos);
		}
		
		thSearch = new ThSearchSystems();
		thSearch.start();
	}
    
    /**
     * 
     */
    class ThSearchSystems extends Thread {
    	private AtomicBoolean running = new AtomicBoolean(false);
    	int numCores = Runtime.getRuntime().availableProcessors();
    	ThreadPoolExecutor executor = (ThreadPoolExecutor) Executors.newFixedThreadPool(numCores*2);
    	
        @Override
        public void run() {
        	running.set(true);
        	
        	Map<Integer, List<ConfigEmu>> systems = getSystemsToScan();
			for (Map.Entry<Integer, List<ConfigEmu>> entry : systems.entrySet()) {
				if (!running.get()) break;
				List<ConfigEmu> emusForSystem = entry.getValue();
				for (ConfigEmu entryentry : emusForSystem) {
					if (!running.get()) break;
					String [] keyValSys = entryentry.getSystem().split("_");
					GameSystem gameSys = new GameSystem(entry.getKey(), keyValSys[1]);
					lblDownSystem.setText(keyValSys[1]);
					findRomsSS(gameSys, entryentry);
				}
			}
			
			running.set(false);
			executor.shutdown();
        }
        
        @Override
        public void interrupt() {
            running.set(false);
        }
        
        public int getActiveCount() {
        	return executor.getActiveCount();
        }
        
        public int getPoolsize() {
        	return executor.getPoolSize();
        }
        

    	
        
        public int queueSize() {
            //return (int)executor.getTaskCount();
            int activeCount = executor.getActiveCount();
            long taskCount = executor.getTaskCount();
            long completedTaskCount = executor.getCompletedTaskCount();
            long tasksToDo = taskCount - completedTaskCount - activeCount;
            return (int)tasksToDo;
        }
        
        /**
         * 
         * @param gameSystem
         * @param cfgEmu
         */
        void findRomsSS(GameSystem gameSystem, ConfigEmu cfgEmu) {
        	URIBuilder uriBuilder = null;
    		int romsDownloaded = 0;
    		iMediasDownloaded = 0;

        	try {
        		final int totalRomsToDownload = getTotalRomsToDownload();
        		lblDownProgress.setText(rb.getString("lbl.media.progress"));
        		
        		Map<String, String> hashAssetsDir = new HashMap<>();
        		hashAssetsDir.put(SS, Utils.getAbsolutePath(cfgEmu.getAssets() + File.separator + DIR_SNAP, txtPrefix.getText()));
        		hashAssetsDir.put(SSTITLE, Utils.getAbsolutePath(cfgEmu.getAssets() + File.separator + DIR_SNAPTIT, txtPrefix.getText()));
        		hashAssetsDir.put(BOX_2D, Utils.getAbsolutePath(cfgEmu.getAssets() + File.separator + DIR_BOX2D, txtPrefix.getText()));
        		hashAssetsDir.put(SYNOPSIS, Utils.getAbsolutePath(cfgEmu.getAssets() + File.separator + DIR_SYNOPSIS, txtPrefix.getText()));
        		
        		for (Map.Entry<String, String> entry : hashAssetsDir.entrySet()) {
        		    String dirpath = entry.getValue();
        		    File dirFileSnap = new File(dirpath);
            		if (!dirFileSnap.exists() && !dirFileSnap.mkdirs()) {
            			logger.log(Level.SEVERE, String.format(rb.getString("alert.error.infodir"), dirpath));
                		return;
            		}
        		}
        		
    			List<Rom> roms;
    			
    			if (cfgEmu.isUse_rom_file()) {
    				String directorioMap  = Utils.getAbsolutePath(cfgEmu.getMap_file(), txtPrefix.getText() + File.separator + GMENU + File.separator + CONFIG);
        			roms = readFile(directorioMap);
    			} else {
    				String dirRoms = Utils.getAbsolutePath(cfgEmu.getRom_directory() ,txtPrefix.getText());
    				roms = readDir(dirRoms);
    			}
    			
    			
    			for (Rom rom : roms) {
    				if (!running.get()) return;
    				String name = Utils.getFileNameWithoutExtension(rom.getShortFileName());
    				
    				if (allFilesDownloaded(hashAssetsDir, name) && chkIgnore.isSelected()) {
    					continue;
    				}
    				
    				romsDownloaded++;
    				lblDownTotalRoms.setText(String.format("%d "+ rb.getString("lbl.of") + " %d - %s", romsDownloaded, totalRomsToDownload, rom.getLongFileName()));
    				
    				uriBuilder = new URIBuilder(SCREENSCRAPERAPIURL + "jeuInfos.php");
    				for (Map.Entry<String, String> entry : fixedParms.entrySet()) {
    				    uriBuilder.addParameter(entry.getKey(), entry.getValue());
    				}
    				
    				uriBuilder.addParameter("systemeid", gameSystem.getId().toString());
    				uriBuilder.addParameter("romtype", "rom");
    				uriBuilder.addParameter("romnom", gameSystem.getId() == MAME_SYS_ID ? name : rom.longFileName.replace("\"", ""));
    				logger.log(Level.FINE, String.format("searching rom %s; url: %s", rom.longFileName, uriBuilder.toString()) );
    				
    				HttpUtil httpUtil = new HttpUtil();
    				httpUtil.get(uriBuilder.toString(), null);
    				String xmlResp = httpUtil.getDataReceived().toString(StandardCharsets.UTF_8);
    				
    				if (xmlResp.isEmpty()) {
    					httpUtil.closeDataReceived();
//    					if (httpUtil.getDownloadErrors() == HttpConst.DownloadErrors.IO_EXCEPTION) {
//    						JOptionPane.showMessageDialog(frame, "Se ha alcanzado el límite diario.\nVuelva a intentarlo más tarde");
//    						break;
//    					}
    					continue;
    				}
    				
    				ByteArrayInputStream input = new ByteArrayInputStream(xmlResp.getBytes(StandardCharsets.UTF_8));
    				try {
    					GameInfo gameInfo = parseGameData(input);
    					for (Map.Entry<String, String> entry : hashAssetsDir.entrySet()) {
    		    		    String key = entry.getKey();
    		    		    String dir = entry.getValue();
    		    		    
    		    		    if (SYNOPSIS.equals(key)) {
    		    		    	writeSynopsis(gameInfo, name, dir);
    		    		    } else if (gameInfo.getImages().containsKey(key)) {
    							GameImage gameImage = gameInfo.getImages().get(key);
    							String ext = gameImage.getFiletype().isEmpty() ? "png" : gameImage.getFiletype();
    			    		    File f = new File(dir + File.separator + name + "." + ext);
    			    		    if(f.exists() && !f.isDirectory() && chkIgnore.isSelected()) { 
    							    continue;
    							}
    							logger.log(Level.FINE, String.format("image ss: %s", gameImage.getUrl()));
    							executor.submit(() -> {
    								downloadImage(gameImage, rom, dir);
    								return null;
    							});
    						} 
    					}
    				} catch (FileNotFoundException | ParserConfigurationException | SAXException e) {
    					logger.log(Level.SEVERE, String.format("Rom not found: %s for url %s", e, uriBuilder.toString()));
    				} finally {
    					httpUtil.closeDataReceived();
    				}
    			}
    			
    			int counts = 0;
                while (!executor.isTerminated() && counts < 10) {
                	TimeUnit.SECONDS.sleep(1);
                	counts++;
                }
                
    		} catch (IOException | URISyntaxException e) {
    			logger.log(Level.SEVERE, String.format("Error findRomsSS: %s for url %s", e.getMessage(), uriBuilder.toString()));
    		} catch (InterruptedException e) {
    			logger.log(Level.SEVERE, String.format("InterruptedException while waiting: %s", e.getMessage()));
    			Thread.currentThread().interrupt();
			} 
        	
            lblDescargandoRecurso.setText(String.format(rb.getString("lbl.media.downloaded"), executor.getCompletedTaskCount()));
        	lblDownTotalRoms.setText("");
        	lblDownProgress.setText(rb.getString("lbl.media.progress.end"));
        }
    }
    
    /**
     * 
     */
    private synchronized void incIMediasDownloaded() {
    	this.iMediasDownloaded++;
    }
    
    /**
     * 
     * @return
     */
    int getTotalRomsToDownload() {
    	int total = 0;
    	try {
			Map<Integer, List<ConfigEmu>> systems = getSystemsToScan();
			for (Map.Entry<Integer, List<ConfigEmu>> entry : systems.entrySet()) {
				List<ConfigEmu> emusForSystem = entry.getValue();
				for (ConfigEmu configEmu : emusForSystem) {
					Map<String, String> hashAssetsDir = new HashMap<>();
					hashAssetsDir.put(SS, Utils.getAbsolutePath(configEmu.getAssets() + File.separator + DIR_SNAP, txtPrefix.getText()));
					hashAssetsDir.put(SSTITLE, Utils.getAbsolutePath(configEmu.getAssets() + File.separator + DIR_SNAPTIT, txtPrefix.getText()));
					hashAssetsDir.put(BOX_2D, Utils.getAbsolutePath(configEmu.getAssets() + File.separator + DIR_BOX2D, txtPrefix.getText()));
					hashAssetsDir.put(SYNOPSIS, Utils.getAbsolutePath(configEmu.getAssets() + File.separator + DIR_SYNOPSIS, txtPrefix.getText()));
					
					String directorioMap  = Utils.getAbsolutePath(configEmu.getMap_file(), txtPrefix.getText() + File.separator + GMENU + File.separator + CONFIG);
					List<Rom> roms;
					
					if (new File(directorioMap).exists()) {
						roms = readFile(directorioMap);
					} else {
						String dirRoms = Utils.getAbsolutePath(configEmu.getRom_directory(), cfgGeneral.getPath_prefix());
						roms = readDir(dirRoms);
					}
					
					for (Rom rom : roms) {
						String name = Utils.getFileNameWithoutExtension(rom.getShortFileName());
						if (allFilesDownloaded(hashAssetsDir, name) && chkIgnore.isSelected()) {
							continue;
						}
						total++;
					}
				}
			}
		} catch (IOException e) {
			logger.log(Level.SEVERE, String.format("Error getTotalRomsToDownload: %s", e.getMessage()));
		}
		return total;
    }

    /**
     * 
     * @param hashAssetsDir
     * @param name
     * @return
     */
	private boolean allFilesDownloaded(Map<String, String> hashAssetsDir, String name) {
		int assetsFound = 0;
		for (Map.Entry<String, String> entry : hashAssetsDir.entrySet()) {
		    String key = entry.getKey();
		    String dir = entry.getValue();
		    File f = new File(dir + File.separator + name + (SYNOPSIS.equals(key) ? ".txt" : ".png"));
		    if(f.exists() && !f.isDirectory()) { 
		    	assetsFound++;
			}
		}
		return assetsFound == hashAssetsDir.size();
	}
	
	/**
     * 
     */
    class ResizeListener extends ComponentAdapter {
		@Override
		public void componentResized(ComponentEvent e) {
			pnlMain.setBounds(0, 0, e.getComponent().getWidth(), e.getComponent().getHeight());
			pnlMain.doLayout();
		}
	}
    
    /**
     * 
     * @param file
     * @return
     * @throws FileNotFoundException
     * @throws IOException
     */
    public List<Rom> readFile(String file) throws FileNotFoundException, IOException {
    	List<Rom> roms = new ArrayList<>();
    	
        try (BufferedReader br
          = new BufferedReader(new FileReader(new File(file)))) {
            String line;
            while ((line = br.readLine()) != null) {
            	Rom rom = new Rom();
            	int firstSpace = line.indexOf(" ");
            	if (firstSpace >= 0) {
            		rom.setShortFileName(line.substring(0, firstSpace));
                    rom.setLongFileName(line.substring(firstSpace + 1).trim());
                    roms.add(rom);
            	}
            }
        }
        return roms;
    }
    
    /**
     * 
     * @param dir
     * @return
     * @throws IOException
     */
    public List<Rom> readDir(String dir) throws IOException {
    	List<Rom> roms = new ArrayList<>();
    	File dirRoms = new File (dir);
    	for (File file : dirRoms.listFiles()) {
        	Rom rom = new Rom();
    		rom.setShortFileName(file.getName());
            rom.setLongFileName(file.getName());
            roms.add(rom);
        }
        return roms;
    }

	/**
	 * 
	 * @param gameInfo
	 * @param name
	 * @param dir
	 */
	private void writeSynopsis(GameInfo gameInfo, String name, String dir) {
		File txtfile = new File(dir + File.separator + name + "." + "txt");
		try (Writer writer = new BufferedWriter(new OutputStreamWriter(
		        new FileOutputStream(txtfile), StandardCharsets.UTF_8))){
			for (String lang : synopsisLangToFind) {
				if (gameInfo.getSynopsis().get(lang) != null) {
					writer.write(gameInfo.getSynopsis().get(lang).getText());
					break;
				}
			}
		} catch (IOException e) {
			logger.log(Level.SEVERE, String.format("Error writing synopsis: %s", txtfile));
		}
	}
    
	
	public static BufferedImage rgbaToIndexedBufferedImage(BufferedImage sourceBufferedImage) {
	    // With this constructor, we create an indexed buffered image with the same dimension and with a default 256 color model
	    BufferedImage indexedImage = new BufferedImage(sourceBufferedImage.getWidth(), sourceBufferedImage.getHeight(), BufferedImage.TYPE_BYTE_INDEXED);
	    ColorModel cm = indexedImage.getColorModel();
	    IndexColorModel icm = (IndexColorModel) cm;

	    int size = icm.getMapSize();

	    byte[] reds = new byte[size];
	    byte[] greens = new byte[size];
	    byte[] blues = new byte[size];
	    //byte[] alphas = new byte[size];
	    icm.getReds(reds);
	    icm.getGreens(greens);
	    icm.getBlues(blues);
	    //icm.getAlphas(alphas);

	    WritableRaster raster = indexedImage.getRaster();
	    //int pixel = raster.getSample(0, 0, 0);
	    //IndexColorModel icm2 = new IndexColorModel(8, size, reds, greens, blues, pixel);
	    //IndexColorModel icm2 = new IndexColorModel(8, size, reds, greens, blues, alphas);
	    IndexColorModel icm2 = new IndexColorModel(8, size, reds, greens, blues, null);
	    indexedImage = new BufferedImage(icm2, raster, sourceBufferedImage.isAlphaPremultiplied(), null);
	    indexedImage.getGraphics().drawImage(sourceBufferedImage, 0, 0, null);
	    return indexedImage;
	}
	
    /**
     * 
     * @param gameImage
     * @param rom
     */
    public void downloadImage(GameImage gameImage, Rom rom, String dirImg) {
		lblDescargandoRecurso.setText(String.format(rb.getString("lbl.download.progress"), thSearch.getActiveCount(), iMediasDownloaded));
    	HttpUtil httpUtil = new HttpUtil();
		httpUtil.get(gameImage.getUrl(), null);
		String name = Utils.getFileNameWithoutExtension(rom.getShortFileName());

    	try {
			if( ((ImgConversion)comboImgConv.getSelectedItem()).getId() == IMG_CONVERSIONS.PNG_INDEXED.value
					|| chkMediaSize.isSelected()){
				
					ByteArrayInputStream bais = new ByteArrayInputStream(httpUtil.getDataReceived().toByteArray());  
					BufferedImage image = ImageIO.read(bais);	
					
					if (chkMediaSize.isSelected()) {
						image = Thumbnails.of(image)
						        .size(Integer.parseInt(txtMediaW.getText()), Integer.parseInt(txtMediaH.getText()))
						        //.dithering(Dithering.DEFAULT)
						        //.imageType(BufferedImage.TYPE_USHORT_565_RGB)
						        //.imageType(BufferedImage.TYPE_INT_ARGB)
						        //.imageType(BufferedImage.TYPE_BYTE_INDEXED)
						        .asBufferedImage();
					}
					
					if (((ImgConversion)comboImgConv.getSelectedItem()).getId() == IMG_CONVERSIONS.PNG_INDEXED.value){
						image = rgbaToIndexedBufferedImage(image);
					}
					ImageIO.write(image, gameImage.getFiletype(), new File(dirImg + File.separator + name + "." + gameImage.getFiletype()));
					image.flush();
					bais.close();
			} else {
				httpUtil.writeToFile(dirImg + File.separator + name + "." + gameImage.getFiletype());
			}
			httpUtil.closeDataReceived();
			lblDescargandoRecurso.setText(String.format(rb.getString("lbl.download.progress"), thSearch.getActiveCount(), iMediasDownloaded));
			incIMediasDownloaded();
		} catch (IOException e) {
			logger.log(Level.SEVERE, String.format("Error descargando imagen %s. %s", name, e.getMessage()));
		}
    }
    
    /**
     * 
     * @param input
     * @return
     * @throws ParserConfigurationException
     * @throws SAXException
     * @throws IOException
     */
    public GameInfo parseGameData(ByteArrayInputStream input) throws ParserConfigurationException, SAXException, IOException {
    	DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
		DocumentBuilder builder = factory.newDocumentBuilder();
		Document document = builder.parse(input);
		Element root = document.getDocumentElement();
		NodeList nList = root.getElementsByTagName("jeu");
		
		GameInfo gameInfo = new GameInfo();
		gameInfo.setImages(new HashMap<>());
		gameInfo.setSynopsis(new HashMap<>());
		
		for (int temp = 0; temp < nList.getLength(); temp++) {
			Node node = nList.item(temp);
			if (node.getNodeType() == Node.ELEMENT_NODE) {
				// Print each employee's detail
				Element eElement = (Element) node;
				findMedias(gameInfo, eElement);
				findSynopsis(gameInfo, eElement);
			}
		}
		return gameInfo;
    }
    
    /**
     * 
     * @param gameInfo
     * @param eElement
     */
	private void findMedias(GameInfo gameInfo, Element eElement) {
		if (eElement.getElementsByTagName(MEDIAS) != null
				&& eElement.getElementsByTagName(MEDIAS).item(0) != null
				&& eElement.getElementsByTagName(MEDIAS).item(0).getChildNodes() != null
				&& eElement.getElementsByTagName(MEDIAS).item(0).getChildNodes().getLength() > 0) {
			NodeList mediaList = eElement.getElementsByTagName(MEDIAS).item(0).getChildNodes();
			
			GameImage boxImage = new GameImage();
			int lastRegionIndex = Integer.MAX_VALUE;
			
			for (int imedia = 0; imedia < mediaList.getLength(); imedia++) {
				Node nodeName = mediaList.item(imedia);
				if (nodeName.getNodeType() == Node.ELEMENT_NODE) {
					Element mediaElement = (Element) nodeName;
					if (imagesToFind.contains(mediaElement.getAttribute(TYPE))) {
						if (BOX_2D.equals(mediaElement.getAttribute(TYPE)) &&
								imagesToFind.contains(mediaElement.getAttribute(REGION)) && 
								regionToFind.indexOf(mediaElement.getAttribute(REGION)) < lastRegionIndex) {
							lastRegionIndex = regionToFind.indexOf(mediaElement.getAttribute(REGION));
							boxImage.setFiletype(mediaElement.getAttribute(FORMAT));
							boxImage.setUrl(mediaElement.getTextContent());
						} else {
							GameImage image = new GameImage();
							image.setFiletype(mediaElement.getAttribute(FORMAT));
							image.setUrl(mediaElement.getTextContent());
							gameInfo.getImages().put(mediaElement.getAttribute(TYPE), image);
						}
					}
				}
			}
			
			if (lastRegionIndex < Integer.MAX_VALUE) {
				gameInfo.getImages().put(BOX_2D, boxImage);
			}
		}
	}
	
	/**
	 * 
	 * @param gameInfo
	 * @param eElement
	 */
	private void findSynopsis(GameInfo gameInfo, Element eElement) {
		if (eElement.getElementsByTagName(SYNOPSIS) != null
				&& eElement.getElementsByTagName(SYNOPSIS).item(0) != null
				&& eElement.getElementsByTagName(SYNOPSIS).item(0).getChildNodes() != null
				&& eElement.getElementsByTagName(SYNOPSIS).item(0).getChildNodes().getLength() > 0) {
			NodeList mediaList = eElement.getElementsByTagName(SYNOPSIS).item(0).getChildNodes();
			for (int imedia = 0; imedia < mediaList.getLength(); imedia++) {
				Node nodeName = mediaList.item(imedia);
				if (nodeName.getNodeType() == Node.ELEMENT_NODE) {
					Element mediaElement = (Element) nodeName;
					if (synopsisLangToFind.contains(mediaElement.getAttribute(LANGUE))) {
						Synopsis synopsis = new Synopsis();
						synopsis.setLang(mediaElement.getAttribute(LANGUE));
						synopsis.setText(mediaElement.getTextContent());
						gameInfo.getSynopsis().put(mediaElement.getAttribute(LANGUE), synopsis);
					}
				}
			}
		}
	}
    
    /**
     * 
     * @param combo
     * @return
     */
    public int fillSystems(JComboBox<GameSystem> combo) {
    	List<GameSystem> systems = getSystems();
    	DefaultComboBoxModel<GameSystem> comboBoxModel = new DefaultComboBoxModel<>();
    	if (!systems.isEmpty()) {
    		comboBoxModel.addElement(new GameSystem(-1, rb.getString("cmb.system")));
    	}
    	for (GameSystem sys : systems) {
    		comboBoxModel.addElement(sys);
    	}
    	combo.setModel(comboBoxModel);
    	return systems.size();
    }
    
    /**
     * 
     * @return
     */
    public List<GameSystem> getSystems(){
    	List<GameSystem> systems = new ArrayList<>();
    	try {
    		File systemsFile = new File(txtPrefix.getText() + File.separator + GMENU 
    				+ File.separator + CONFIG + File.separator + "systems.xml");
    		String xmlResp;
    		
    		if (!systemsFile.exists()) {
    			URIBuilder uriBuilder = new URIBuilder(SCREENSCRAPERAPIURL + "systemesListe.php");
    			for (Map.Entry<String, String> entry : fixedParms.entrySet()) {
    			    uriBuilder.addParameter(entry.getKey(), entry.getValue());
    			}
    			
    			HttpUtil httpUtil = new HttpUtil();
    			httpUtil.get(uriBuilder.toString(), null);
    			xmlResp = httpUtil.getDataReceived().toString(StandardCharsets.UTF_8);
    			
    			try (FileWriter writer = new FileWriter(systemsFile)) {
    	            writer.write(xmlResp);
    	        }
    			httpUtil.closeDataReceived();
    		} else {
    			try (BufferedReader br = new BufferedReader(new FileReader(systemsFile))) {
    			    StringBuilder sb = new StringBuilder();
    			    String line;
    			    while ((line = br.readLine()) != null) {
    			        sb.append(line).append("\n");
    			    }
    			    xmlResp = sb.toString();
    			}
    		}
			
			DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
			DocumentBuilder builder = factory.newDocumentBuilder();
			ByteArrayInputStream input = new ByteArrayInputStream(xmlResp.getBytes(StandardCharsets.UTF_8));
			Document document = builder.parse(input);
			Element root = document.getDocumentElement();
			
			NodeList nList = root.getElementsByTagName("systeme");
			for (int temp = 0; temp < nList.getLength(); temp++) {
				  Node node = nList.item(temp);
			      if (node.getNodeType() == Node.ELEMENT_NODE) {
				        Element eElement = (Element) node;
				        Integer systemId = -1;
				        if (eElement.getElementsByTagName("id") != null && eElement.getElementsByTagName("id").getLength() > 0) {
				        	systemId = Integer.parseInt(eElement.getElementsByTagName("id").item(0).getTextContent());
				        }
				        
				        String systemName = "";
				        if (eElement.getElementsByTagName("noms") != null && eElement.getElementsByTagName("noms").item(0).getChildNodes() != null
				        		&& eElement.getElementsByTagName("noms").item(0).getChildNodes().getLength() > 0) {
				        	NodeList nomsList = eElement.getElementsByTagName("noms").item(0).getChildNodes();
				        	for (int iNames = 0; iNames < nomsList.getLength(); iNames++) {
				        		Node nodeName = nomsList.item(iNames);
				        		if (nodeName.getNodeType() == Node.ELEMENT_NODE) {
				        			Element nameElement = (Element) nodeName;
				        			if ("nom_eu".equals(nameElement.getNodeName())) {
				        				systemName = nameElement.getTextContent();
				        			}
				        		}
				        	}
				        }
				         
				        if (systemId > -1 && !systemName.isEmpty()) {
				        	GameSystem gs = new GameSystem();
					        gs.setId(systemId);
					        gs.setName(systemName);
					        systems.add(gs);
				        }
			      }
			}
			systems.sort(Comparator.comparing(GameSystem::getName));
		} catch (URISyntaxException | SAXException | IOException | ParserConfigurationException e) {
			logger.log(Level.SEVERE, "Error obtaining the systems", e);
		}
    	return systems;
    }
    
    /**
     * 
     */
	public void addEmu() {
		String emuId = (String) JOptionPane.showInputDialog(pnlGeneral, rb.getString("alert.error.emuid"),
				"Aviso", JOptionPane.QUESTION_MESSAGE, null, null, null);
		
		while (mapEmus.containsKey(emuId) && !emuId.isEmpty()) {
			String str = String.format(rb.getString("lbl.emuexist"), emuId);
			emuId = (String) JOptionPane.showInputDialog(pnlGeneral, str, "Error", JOptionPane.ERROR_MESSAGE, null, null, null);
		}
		
		if (emuId == null || emuId.isEmpty())
			return;
		
		ConfigEmu configEmu = new ConfigEmu();
		configEmu.setMap_file(emuId + ".map");
		configEmu.setAssets(GMENU + File.separator + ASSETS + File.separator + emuId);
		mapEmus.put(emuId, configEmu);
		addTabEmu(emuId);
	}
	
	/**
	 * 
	 * @param panel
	 * @param name
	 * @return
	 */
	public Component findByNameInPanel(JPanel panel, String name) {
		Component[] components = panel.getComponents();
        for (Component comp : components) {
        	if (name.equals(comp.getName()) && comp instanceof JTextField) {
        		return comp;
        	}
        }
        return null;
	}
	
	public boolean getCheckValueInPanel(JPanel panel, String name) {
		Component[] components = panel.getComponents();
        for (Component comp : components) {
        	if (name.equals(comp.getName()) && comp instanceof JCheckBox) {
        		return ((JCheckBox)comp).isSelected();
        	}
        }
        return false;
	}
	
	public JComboBox<?> getComboInPanel(JPanel panel, String name) {
		Component[] components = panel.getComponents();
        for (Component comp : components) {
        	if (name.equals(comp.getName()) && comp instanceof JComboBox<?>) {
        		return ((JComboBox<?>)comp);
        	}
        }
        return null;
	}
	
	/**
	 * 
	 * @param title
	 * @param tab
	 * @return
	 */
	public int findTabByName(String title, JTabbedPane tab) {
		int tabCount = tab.getTabCount();
		for (int i = 0; i < tabCount; i++) {
			if (tab.getTitleAt(i).equals(title))
				return i;
		}
		return -1;
	}
	
	/**
	 * 
	 * @param emuId
	 */
	public void addTabEmu(String emuId) {
		ConfigEmu cfgEmu = mapEmus.get(emuId);
		
		int tabIdx = -1;
		if ((tabIdx = findTabByName(emuId, tabsMain)) >= 0) {
			tabsMain.remove(tabIdx);
		}

		JPanel newPanel = new JPanel();
		tabsMain.addTab(emuId, null, newPanel, rb.getString("lbl.tab") + " " + emuId);
		newPanel.setLayout(null);
		
		int y = 0; 
		int x = 20;
		final int sepY = 10;
		final int fieldW = 320;
		final int jumpX = fieldW + 50;
		final int jumpY = 20;
		
		Class<ConfigEmu> c = ConfigEmu.class;
        Field[] fields = c.getDeclaredFields();
        
        for(int i = 0; i < fields.length; i++) {
        	try {
        		TabElementEmuCfg infoEmuCfg = hashTabElements.get(fields[i].getName());
				Field f = c.getDeclaredField(fields[i].getName());
        		if (infoEmuCfg == null || !infoEmuCfg.isVisible() || f == null) {
        			continue;
        		}
        		f.setAccessible(true);
        		
				if (y + jumpY*2 + 25 > pnlGeneral.getHeight()) {
					y = 0;
					x += jumpX;
				}
				
				if (infoEmuCfg.isListSelector()) {
					JLabel lbl = new JLabel(infoEmuCfg.getLabel());
					lbl.setName(fields[i].getName());
					lbl.setBounds(x, y +=jumpY + sepY, fieldW, 15);
					newPanel.add(lbl);
					
					JComboBox<GameSystem> cmb = new JComboBox<>();
					cmb.setName(fields[i].getName());
					cmb.setBounds(x, y +=jumpY, fieldW, 25);
					cmb.setToolTipText(infoEmuCfg.getHelp());
					if (SYSTEM.equals(fields[i].getName())) {
						fillSystems(cmb);
					}
					
					//Find the selected value
					String emuSel = (String) f.get(cfgEmu);
					if (emuSel != null && !emuSel.isEmpty()) {
						String [] arrEmuSel = emuSel.split("_");
						for (int cIdx = 0; cIdx < cmb.getItemCount(); cIdx++) {
						    Object item = cmb.getItemAt(cIdx);
						    if (arrEmuSel.length > 1 && item.toString().equals(arrEmuSel[1].trim())) {
						    	cmb.setSelectedIndex(cIdx);
						        break;
						    }
						}
					}
					newPanel.add(cmb);
					lbl.setLabelFor(cmb);
				} else if (fields[i].getType().isAssignableFrom(String.class)) {
					y +=jumpY + sepY;
					JLabel lbl = new JLabel(infoEmuCfg.getLabel());
					lbl.setName(fields[i].getName());
					lbl.setBounds(x, y, fieldW, 15);
					lbl.setToolTipText(infoEmuCfg.getHelp());
					//lbl.getBounds().x = x;
					//lbl.getBounds().y = y;
					newPanel.add(lbl);
					
					JTextField txt = new JTextField(fields[i].getName());
					txt.setName(fields[i].getName());
					txt.setBounds(x, y +=jumpY, fieldW, 25);
					txt.setColumns(10);
					txt.setText((String) f.get(cfgEmu));
					txt.setToolTipText(infoEmuCfg.getHelp());
					newPanel.add(txt);
					lbl.setLabelFor(txt);
					
					if (infoEmuCfg.isFileSelector()) {
						JButton btn = new JButton("...");
						btn.setBounds(x + fieldW + 10, y, 25, 25);
						newPanel.add(btn);
						setBtnTextActions(btn, txt, newPanel, emuId);
					}
					
					if (infoEmuCfg.isMapBtn()) {
						JButton btn = new JButton();
						btn.setBounds(x + fieldW + 20 + 25, y, 25, 25);
						BufferedImage image;
						InputStream icoStream = Utils.loadResource("img/Msdos.png");
						if (icoStream != null) {
							try {
								image = ImageIO.read(icoStream);
								Image scaledImage = image.getScaledInstance(btn.getWidth(), btn.getHeight(), java.awt.Image.SCALE_SMOOTH);
								iconMsDos = new ImageIcon(scaledImage);
								btn.setIcon(iconMsDos);
							} catch (IOException e) {
								logger.warning("Could not load an msdos icon");
							}
						}
						
						btn.setToolTipText(rb.getString("lbl.mapping"));
						btn.setMinimumSize(new Dimension(25,25));
						btn.setPreferredSize(new Dimension(25,25));
						btn.setSize(new Dimension(25,25));
						newPanel.add(btn);
						btn.addActionListener( e -> {
							generateMapRoms(txt, emuId);
						});
					}
				} else if (fields[i].getType().isAssignableFrom(boolean.class)) {
					JCheckBox chk = new JCheckBox(infoEmuCfg.getLabel());
					chk.setName(fields[i].getName());
					chk.setBounds(x, y +=jumpY + sepY, fieldW, 25);
					chk.setSelected((Boolean)f.get(cfgEmu));
					chk.setToolTipText(infoEmuCfg.getHelp());
					newPanel.add(chk);
				}
			} catch (SecurityException | IllegalArgumentException | IllegalAccessException | NoSuchFieldException e) {
				logger.log(Level.SEVERE, "Error adding components", e);
			} 
        }
        
        y += jumpY + sepY;
        JButton btnSaveEmu = new JButton();
        btnSaveEmu.setIcon(iconSave);
        btnSaveEmu.setBounds(x, y, 25, 25);
		newPanel.add(btnSaveEmu);
		
		btnSaveEmu.addActionListener( e -> {
			Component tabContent = tabsMain.getSelectedComponent();
			
	        if (tabContent instanceof JPanel) {
	            JPanel panel = (JPanel) tabContent;
	            Component[] components = panel.getComponents();
	            for (Component comp : components) {
	            	try {
            			Field f = c.getDeclaredField(comp.getName());
            			f.setAccessible(true);

	            		if (comp instanceof JComboBox) { 
	            			Object sel = ((JComboBox<?>)comp).getSelectedItem();
	            			String text = "";
	            			if (sel instanceof GameSystem && ((GameSystem)sel).getId() != -1) {
	            				text = ((GameSystem)sel).getId() + "_" + ((GameSystem)sel).getName();
	            			}
							if (f.getType().isAssignableFrom(String.class)) {
								f.set(cfgEmu, text);
							}
							logger.info(String.format("%s: %s = %s", comp.getClass().getName(), comp.getName(), text));
	            			
	            		} else if (comp instanceof JTextField) { 
							String text = ((JTextField)comp).getText();
							if (f.getType().isAssignableFrom(String.class)) {
								f.set(cfgEmu, text);
							}
							logger.info(String.format("%s: %s = %s", comp.getClass().getName(), comp.getName(), text));
						} else if (comp instanceof JCheckBox) {
							boolean selected = ((JCheckBox)comp).isSelected();
							if (f.getType().isAssignableFrom(boolean.class)) {
								f.set(cfgEmu, selected);
							}
							logger.info(String.format("%s: %s = %s", comp.getClass().getName(), comp.getName(), selected));
						}
					} catch (NoSuchFieldException e1) {
						logger.log(Level.WARNING, "No existe el campo {}", e);
					} catch (Exception e1) {
						logger.log(Level.SEVERE, "Error asignando campos {}", e);
					}
	            }
	            
	            cfgEmu.setScreen_shot_directory(cfgEmu.getAssets() + File.separator + DIR_SNAP);
	            
	            logger.log(Level.INFO, "Component of emu created");
	            mapEmus.replace(emuId, cfgEmu);
	            File cfgFile = new File(txtPrefix.getText() + File.separator + GMENU + File.separator + CONFIG
	            		+ File.separator + emuId + ".cfg");
	            
	            CfgFile.saveConfig(cfgFile, cfgEmu, hashTabElements);
	        }
		});
	}
	
	/**
	 * 
	 * @param btn
	 * @param txt
	 * @param newPanel
	 * @param emuId
	 */
	private void setBtnTextActions(JButton btn, JTextField txt, JPanel newPanel, String emuId) {
		btn.addActionListener( arg0 -> {
			try {
				URI baseUri = new URI(txtPrefix.getText());
				JFileChooser chooser = new JFileChooser();
				chooser.setFileSelectionMode(JFileChooser.FILES_AND_DIRECTORIES);
				chooser.setAcceptAllFileFilterUsed(true);
				chooser.setCurrentDirectory(new File(txtPrefix.getText()));
				
				if (MAP_FILE.equals(txt.getName())) {
					baseUri = new URI(txtPrefix.getText() + File.separator + GMENU + File.separator + CONFIG);
					FileNameExtensionFilter filter = new FileNameExtensionFilter("Map rom files", "map");
					chooser.addChoosableFileFilter(filter);
					chooser.setFileFilter(filter);
					chooser.setFileSelectionMode(JFileChooser.FILES_ONLY);
					chooser.setCurrentDirectory(new File(baseUri.getPath()));
				} else if (EXECUTABLE.equals(txt.getName())){
					chooser.setFileSelectionMode(JFileChooser.FILES_ONLY);
				} else if (ASSETS.equals(txt.getName())) {
					chooser.setFileSelectionMode(JFileChooser.DIRECTORIES_ONLY);
					chooser.setCurrentDirectory(new File(txtPrefix.getText() + File.separator + GMENU + File.separator + ASSETS));
				}
				
				int returnVal = chooser.showOpenDialog(tabsMain);
				if(returnVal == JFileChooser.APPROVE_OPTION) {
					File file = new File(chooser.getSelectedFile().getAbsolutePath());
					
					if (DIRECTORY.equals(txt.getName())) {
						if (file.isFile()) {
							txt.setText(baseUri.relativize(new URI(file.getParent())).getPath());
							((JTextField)findByNameInPanel(newPanel, EXECUTABLE)).setText(file.getName());
						} else {
							txt.setText(baseUri.relativize(new URI(file.getAbsolutePath())).getPath());
						}
					} else if (EXECUTABLE.equals(txt.getName()) && file.isFile()) {
						txt.setText(file.getName());
						((JTextField)findByNameInPanel(newPanel, DIRECTORY)).setText(baseUri.relativize(new URI(file.getParent())).getPath());
					} else if (ASSETS.equals(txt.getName()) || ROM_DIRECTORY.equals(txt.getName())) {
						//URI uri = new URI(file.isFile() ? file.getParent() : file.getAbsolutePath());
						//txt.setText(baseUri.relativize(uri).getPath());
						String filepath = file.isFile() ? file.getParent() : file.getAbsolutePath();
						if (filepath.contains(baseUri.getPath())) {
							txt.setText(filepath.substring(baseUri.getPath().length() + 1));
						} else {
							txt.setText(filepath);
						}
					} else if (MAP_FILE.equals(txt.getName()) && file.isFile()) {
						txt.setText(baseUri.relativize(new URI(file.getAbsolutePath())).getPath());
					}
				}
			} catch (HeadlessException | URISyntaxException e) {
				logger.log(Level.SEVERE, e.getMessage());
			} 
		});
	}
	
	/**
	 * 
	 */
	private void createConfig() {
		boolean fileCreated = true;
		File cfgDir = new File(txtPrefix.getText() + File.separator + GMENU);
		File cfgFile = new File(txtPrefix.getText() + File.separator + GMENU + File.separator + GMENU_CFG);
		
		if (!cfgDir.exists() && !cfgDir.mkdirs()) {
			JOptionPane.showMessageDialog(frame, rb.getString("alert.error.dirnotwrite"));
			return;
		}
		
		try {
			if (!cfgFile.exists()) {
				fileCreated = cfgFile.createNewFile();
			}
		} catch (IOException e) {
			JOptionPane.showMessageDialog(frame, rb.getString("alert.error.dircfgnotwrite"));
			return;
		}
		
		txtDirReplaced.setText(txtPrefix.getText());
		
		if (fileCreated) {
			//StringBuilder emulators = new StringBuilder("");
			List<String> emulators = new ArrayList<>();
			for (Map.Entry<String, ConfigEmu> entry : mapEmus.entrySet()) {
				//emulators.append(emulators.length() == 0 ? entry.getKey() : " " + entry.getKey());
				emulators.add(entry.getKey());
			}
			Collections.sort(emulators);
			
			//clears file every time
			try (Writer writer = new BufferedWriter(new OutputStreamWriter(
			        new FileOutputStream(cfgFile), StandardCharsets.UTF_8))){
				final String newLine = System.getProperty(LINE_SEPARATOR);
				writer.write("[Main]" + newLine);
				writer.write("emulators = " + String.join(" ", emulators) + newLine);
				writer.write("debug = " + checkCfgVal(chkDebug) + newLine);
				writer.write("resolution = " + txtW.getText() + " " + txtH.getText() + newLine);
				boolean hasLastSep = txtPrefix.getText().length() > 0 && 
						File.separator.equals(Character.toString(txtPrefix.getText().charAt(txtPrefix.getText().length() - 1)));
				writer.write("path_prefix = " + convertDir(txtPrefix.getText() + (!hasLastSep ? File.separator : "") + newLine));
				writer.write("convert_enable = " + checkCfgVal(chkConvert) + newLine);
				writer.write("convert_prefix_src = " + txtDirReplaced.getText() + newLine);
				writer.write("convert_prefix_dst = " + txtDirToReplace.getText() + newLine);
				
				File fileCfg = new File(txtPrefix.getText() + File.separator + GMENU + File.separator + GMENU_CFG);
				CfgFile.loadConfig(fileCfg, cfgGeneral);
				
				JOptionPane.showMessageDialog(frame, rb.getString("alert.error.changessaved"));
			} catch (IOException e) {
				JOptionPane.showMessageDialog(frame, rb.getString("alert.error.openfailed"));
			}
		}
	}
	
	/**
	 * 
	 * @param txt
	 * @param emuId
	 */
	private void generateMapRoms(JTextField txt, String emuId) {
		File cfgEmuDir; 
		final String dosFileSep = "\\";
		Component tabContent = tabsMain.getSelectedComponent();
		
        if (tabContent instanceof JPanel) {
        	JTextField txtFieldMap = (JTextField)findByNameInPanel((JPanel)tabContent, MAP_FILE);
        	cfgEmuDir = new File(txtPrefix.getText() + File.separator + GMENU + File.separator 
        			+ CONFIG + File.separator + txtFieldMap.getText());
        	
        	if (cfgEmuDir.exists()) {
        		int result = JOptionPane.showConfirmDialog(frame, rb.getString("alert.error.fileoverwrite"), 
        				"Alerta", JOptionPane.YES_NO_OPTION, JOptionPane.QUESTION_MESSAGE);
        		if(result != JOptionPane.YES_OPTION){
        			return;
        		}
        	}
        	
        	JTextField txtFieldRoms = (JTextField)findByNameInPanel((JPanel)tabContent, ROM_DIRECTORY);
        	String prefix = txtPrefix.getText().trim() + (txtPrefix.getText().trim().endsWith(File.separator) ? "" : File.separator);
        	String tmpRomDir = txtFieldRoms.getText().trim();
        	String romsDirAbsolute = Utils.getAbsolutePath(tmpRomDir, prefix);
        	
        	if (romsDirAbsolute.isEmpty() || !new File(romsDirAbsolute).exists()) {
        		JOptionPane.showMessageDialog(frame, rb.getString("alert.error.pathnotexists"));
        		return;
        	}
        	
        	String romPathDos = "";
        	if (getCheckValueInPanel((JPanel)tabContent, USE_ROM_DIRECTORY) ) {
        		try {
        			DosNames dosNames = new DosNames();
            		JTextField txtFieldEmuDir = (JTextField)findByNameInPanel((JPanel)tabContent, DIRECTORY);
                	String tmpEmuDir = txtFieldEmuDir.getText().trim();
    				URI baseUri = new URI(Utils.getAbsolutePath(tmpEmuDir, prefix));
    				//romDirRelativeToMainEmuExe = baseUri.relativize(new URI(romsDirAbsolute)).getPath().replace("/", dosFileSep) + dosFileSep;
    				//romDirRelativeToMainEmuExe = Utils.relativize(baseUri.getPath(), romsDirAbsolute).replace("/", dosFileSep) + dosFileSep;
    				
    				String baseDos = dosNames.convertPathForDos(baseUri.getPath());
    				String romAbsDos = dosNames.convertPathForDos(romsDirAbsolute);
    				
    				//romDirRelativeToMainEmuExe = Utils.relativize(baseDos, romAbsDos).replace("/", dosFileSep) + dosFileSep;
    				romPathDos = romAbsDos.toLowerCase().
    						replace(txtDirReplaced.getText().toLowerCase(), 
    								txtDirToReplace.getText()).replace("/", dosFileSep);
    				
    				if (dosFileSep.compareTo(romPathDos.substring(romPathDos.length()-1)) != 0) {
    					romPathDos += dosFileSep;
    				}
    				
    			} catch (URISyntaxException e) {
    				JOptionPane.showMessageDialog(frame, rb.getString("alert.error.rompath"));
    				return;
    			}
        	}
        	
        	if (!findTitlesForMame(cfgEmuDir, tabContent, romsDirAbsolute)) {
        		return;
        	}
        	
        	findTitlesForMsdosPaths(cfgEmuDir, tabContent, romsDirAbsolute, romPathDos);
        }
	}

	/**
	 * 
	 * @param cfgEmuDir
	 * @param tabContent
	 * @param romsDirAbsolute
	 * @param romDirRelativeToMainEmuExe
	 */
	private void findTitlesForMsdosPaths(File cfgEmuDir, Component tabContent,
			String romsDirAbsolute, String romDirRelativeToMainEmuExe) {
		DosNames dos = new DosNames();
		String [] filter = ((JTextField)findByNameInPanel((JPanel)tabContent, ROM_EXTENSION)).getText().split(" ");
		final String newLine = System.getProperty(LINE_SEPARATOR);
		
		List<DosNames.FileName8_3> listFiles = dos.convertirNombresCortos(romsDirAbsolute, Arrays.asList(filter).stream()
		        .map(String::toLowerCase).collect(Collectors.toList()));
		
		try (Writer writer = new BufferedWriter(new OutputStreamWriter(new FileOutputStream(cfgEmuDir), StandardCharsets.UTF_8))){
			for (DosNames.FileName8_3 archivo : listFiles) {
				if (!getCheckValueInPanel((JPanel)tabContent, USE_EXTENSION) && archivo.getShortFN().contains(".") 
						&& archivo.getShortFN().length() - 1 - archivo.getShortFN().lastIndexOf(".") <= 3) {
					archivo.setShortFN(archivo.getShortFN().substring(0, archivo.getShortFN().lastIndexOf(".")));
				}
				writer.write(romDirRelativeToMainEmuExe + archivo.getShortFN() + " \""
						+ Utils.getFileNameWithoutExtension(archivo.getLongFN()).trim() + "\"" + newLine);
			}
			String str = String.format(rb.getString("alert.filesavedon"), cfgEmuDir.getAbsolutePath());
			JOptionPane.showMessageDialog(frame, str);
		} catch (IOException e) {
			JOptionPane.showMessageDialog(frame, rb.getString("alert.error.filesavedko"));
		}
	}

	/**
	 * find title if MAME is the system selected. It returns true to continue execution and scrap other systems 
	 * @param cfgEmuDir
	 * @param tabContent
	 * @param romsDirAbsolute
	 * @return
	 */
	private boolean findTitlesForMame(File cfgEmuDir, Component tabContent, String romsDirAbsolute) {
		final String newLine = System.getProperty(LINE_SEPARATOR);
		DosNames dos = new DosNames();
		String [] filter = ((JTextField)findByNameInPanel((JPanel)tabContent, ROM_EXTENSION)).getText().split(" ");
		
		JComboBox<?> comboSystems = getComboInPanel((JPanel)tabContent, SYSTEM);
		if (comboSystems.getSelectedItem() instanceof GameSystem && MAME_SYS_ID == ((GameSystem)comboSystems.getSelectedItem()).getId()) {
			File mamedat = new File(txtPrefix.getText() + File.separator + GMENU + File.separator 
	    			+ CONFIG + File.separator + MAME_DAT);
			if (!mamedat.exists()) {
				JOptionPane.showMessageDialog(frame, String.format(rb.getString("alert.mame"), mamedat.getAbsolutePath()));
				return false;
			}
			
			Map<String, String> mameMap = new HashMap<>();
			try (BufferedReader reader = new BufferedReader(new FileReader(mamedat, StandardCharsets.UTF_8))){
				String line;
				while ((line = reader.readLine()) != null) {
					if (line.indexOf(" ") >= 0) {
						mameMap.put(line.substring(0, line.indexOf(" ")).trim(), line.substring(line.indexOf(" ")).trim().replace("\"", ""));
					}
				}
			} catch (IOException e) {
				JOptionPane.showMessageDialog(frame, rb.getString("alert.error.mame"));
			}
			
			List<DosNames.FileName8_3> listFiles = dos.convertirNombresCortos(romsDirAbsolute, Arrays.asList(filter));
			try (Writer writer = new BufferedWriter(new OutputStreamWriter(new FileOutputStream(cfgEmuDir), StandardCharsets.UTF_8))){
				for (DosNames.FileName8_3 archivo : listFiles) {
					archivo.setShortFN(archivo.getShortFN().substring(0, archivo.getShortFN().lastIndexOf(".")).toLowerCase());
					
					String title = mameMap.get(archivo.getShortFN());
					if (title != null && !title.isEmpty()){
						writer.write(archivo.getShortFN() + " \"" + title + "\"" + newLine);
					} else {
						writer.write(archivo.getShortFN() + " \"" + archivo.getShortFN() + "\"" + newLine);
					}
				}
				JOptionPane.showMessageDialog(frame, String.format(rb.getString("alert.filesavedon"), cfgEmuDir.getAbsolutePath()));
			} catch (IOException e) {
				JOptionPane.showMessageDialog(frame, rb.getString("alert.error.filesavedko"));
			}
			return false;
		}
		return true;
	}
	
	/**
	 * 
	 * @param chck
	 * @return
	 */
	private String checkCfgVal(JCheckBox chck) {
		return chck.isSelected() ? "yes" : "no";
	}
	
	/**
	 * 
	 * @param src
	 * @return
	 */
	private String convertDir(String src) {
		String replaced = src;
		final String WIN_SEPARATOR = "\\";
		if (chkConvert.isSelected()) {
			replaced = src.replace(txtDirReplaced.getText(), txtDirToReplace.getText());
			
			if (txtDirToReplace.getText().contains(WIN_SEPARATOR) && !txtDirToReplace.getText().contains(File.separator)) {
				replaced = replaced.replace(File.separator, WIN_SEPARATOR);
			} else if (txtDirToReplace.getText().contains(File.separator) && !txtDirToReplace.getText().contains(WIN_SEPARATOR)) {
				replaced = replaced.replace(WIN_SEPARATOR, File.separator);
			}
		}
		return replaced;
	}
	
	/**
	 * 
	 */
	private void findScrappableSystems() {
		DefaultListModel<ScrapSystem> listModel = new DefaultListModel<>();
		List<String> listOfAddedElems = new ArrayList<>();
		
		for (Map.Entry<String, ConfigEmu> entry : mapEmus.entrySet()) {
			String [] keyValSys = entry.getValue().getSystem().split("_");
			if (keyValSys.length >= 2) {
				ScrapSystem scrapSystem = new ScrapSystem(Integer.parseInt(keyValSys[0]), keyValSys[1]);
				if (!listOfAddedElems.contains(entry.getValue().getSystem())) {
					listModel.addElement(scrapSystem);
				}
				listOfAddedElems.add(entry.getValue().getSystem());
			}
		}
		sortModel(listModel);
		listSystemsToScrap.setModel(listModel);
		btnBuscar.setEnabled(listModel.size() > 0);
	}
	
	private void sortModel(DefaultListModel<ScrapSystem> model) {
	    List<ScrapSystem> list = new ArrayList<>();
	    for (int i = 0; i < model.size(); i++) {
	        list.add(model.get(i));
	    }
	    Collections.sort(list, (e1, e2) -> e1.getName().compareTo(e2.getName()));
	    
	    model.removeAllElements();
	    for (ScrapSystem s : list) {
	        model.addElement(s);
	    }
	}
	
	/**
	 * 
	 * @return
	 */
	private Map<Integer, List<ConfigEmu>> getSystemsToScan() {
		Map<Integer, List<ConfigEmu>> emuBySystem = new HashMap<>();
		
		for (ScrapSystem modelSystemElem : listSystemsToScrap.getSelectedValuesList()) {
			for (Map.Entry<String, ConfigEmu> entry : mapEmus.entrySet()) {
				if (entry.getValue().getSystem().equals(modelSystemElem.getId() + "_" + modelSystemElem.getName())) {
					if (!emuBySystem.containsKey(modelSystemElem.getId())) {
						List<ConfigEmu> listEmus =  new ArrayList<>();
						logger.log(Level.INFO, String.format("Dowloading for emu %s", entry.getValue().getName()));
						listEmus.add(entry.getValue());
						emuBySystem.put(modelSystemElem.getId(), listEmus);
					} else {
						boolean pathAlreadyFound = false;
						for (ConfigEmu emuInSystem : emuBySystem.get(modelSystemElem.getId())) {
							if (entry.getValue().getRom_directory().equals(emuInSystem.getRom_directory())
									//&& entry.getValue().getMap_file().equals(emuInSystem.getMap_file())
							) {
								pathAlreadyFound = true;
							}
						}
						if (!pathAlreadyFound) {
							logger.log(Level.INFO, String.format("Dowloading for emu %s", entry.getValue().getName()));
							emuBySystem.get(modelSystemElem.getId()).add(entry.getValue());
						} else {
							logger.log(Level.INFO, String.format("Path previously found for emu %s", entry.getValue().getName()));
						}
					}
				}
			}
		}
		return emuBySystem;
	}
}
	