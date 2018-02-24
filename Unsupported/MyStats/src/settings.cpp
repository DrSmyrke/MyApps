static void createConfig(){
	CIniFile IniFile;
	IniFile.SetPath(confFile);
	IniFile.SetValueI("Main","winWidth",270,1);
	IniFile.SetValueI("Main","winHeight",300,1);
	IniFile.SetValueB("Main","winAutohide",1,1);
	IniFile.SetValueB("Main","saveLogs",0,1);
	IniFile.SetValueI("Main","winUpdateTime",500,1);
	IniFile.SetValueI("Main","yIteration",15,1);
	IniFile.KeyComment("Main","winUpdateTime (millisecond)");

	IniFile.SetValue("Background","colorRGB","0,0,0",1);
	IniFile.SetValueF("Background","colorAlpha",0.8,1);
	IniFile.KeyComment("Background","colorRGB [,] - separator (value - 0..255)");

	IniFile.SetValue("Clock","font","Sans",1);
	IniFile.SetValue("Clock","colorRGB","229,191,14",1);
	IniFile.KeyComment("Clock","colorRGB [,] - separator (value - 0..255)");

	IniFile.SetValue("TextStyle","font","Sans",1);
	IniFile.SetValueI("TextStyle","style",0,1);
	IniFile.SetValueI("TextStyle","weight",0,1);
	IniFile.SetValueI("TextStyle","size",12,1);
	IniFile.KeyComment("TextStyle","text style [normal=0, italic=1, oblique=2] text weight [normal=0, bold=1]");

	IniFile.SetValue("Param","colorRGB","255,175,0",1);
	IniFile.SetValueF("Param","colorAlpha",0.9,1);
	IniFile.KeyComment("Param","colorRGB [,] - separator (value - 0..255)");

	IniFile.SetValue("Value","colorRGB","138,186,56",1);
	IniFile.SetValueF("Value","colorAlpha",0.9,1);
	IniFile.KeyComment("Value","colorRGB [,] - separator (value - 0..255)");

	IniFile.SetValue("ProgressBar","colorRGB","100,100,100",1);
	IniFile.SetValueF("ProgressBar","colorAlpha",0.5,1);
	IniFile.SetValueI("ProgressBar","blockWidth",3,1);
	IniFile.SetValueI("ProgressBar","blockHeight",12,1);
	IniFile.SetValueI("ProgressBar","blockIntery",1,1);
	IniFile.KeyComment("ProgressBar","blockIntery (The distance between the blocks)");
	IniFile.KeyComment("ProgressBar","colorRGB [,] - separator (value - 0..255)");

	IniFile.WriteFile();
}
static void loadConfig(){
	CIniFile IniFile;
	IniFile.SetPath(confFile);
	IniFile.ReadFile();

	winWidth=IniFile.GetValueI("Main","winWidth");
	winHeight=IniFile.GetValueI("Main","winHeight");
	winAutohide=IniFile.GetValueB("Main","winAutohide");
	saveLogs=IniFile.GetValueB("Main","saveLogs");
	winUpdateTime=IniFile.GetValueI("Main","winUpdateTime");
	yi=IniFile.GetValueI("Main","yIteration");

	backgroundColor=splitToInt(IniFile.GetValue("Background","colorRGB"),",");
	backgroundColorAlpha=IniFile.GetValueI("Background","colorAlpha");

	clockFont=strdup(IniFile.GetValue("Clock","font").c_str());
	clockColor=splitToInt(IniFile.GetValue("Clock","colorRGB"),",");

	defaultTextFont=strdup(IniFile.GetValue("TextStyle","font").c_str());
	switch (IniFile.GetValueI("TextStyle","style")) {
		case 0: defaultTextStyle=CAIRO_FONT_SLANT_NORMAL; break;
		case 1: defaultTextStyle=CAIRO_FONT_SLANT_ITALIC; break;
		case 2: defaultTextStyle=CAIRO_FONT_SLANT_OBLIQUE; break;
		default: break;
	}
	switch (IniFile.GetValueI("TextStyle","weight")) {
		case 0: defaultTextWeight=CAIRO_FONT_WEIGHT_NORMAL; break;
		case 1: defaultTextWeight=CAIRO_FONT_WEIGHT_BOLD; break;
		default: break;
	}
	defaultTextSize=IniFile.GetValueI("TextStyle","size");

	paramColor=splitToInt(IniFile.GetValue("Param","colorRGB"),",");
	paramColorAlpha=IniFile.GetValueF("Param","colorAlpha");

	valueColor=splitToInt(IniFile.GetValue("Value","colorRGB"),",");
	valueColorAlpha=IniFile.GetValueF("Value","colorAlpha");

	progressBarColor=splitToInt(IniFile.GetValue("ProgressBar","colorRGB"),",");
	progressBarColorAlpha=IniFile.GetValueF("ProgressBar","colorAlpha");
	progressBarBlockWidth=IniFile.GetValueI("ProgressBar","blockWidth");
	progressBarBlockHeight=IniFile.GetValueI("ProgressBar","blockHeight");
	progressBarBlockIntery=IniFile.GetValueI("ProgressBar","blockIntery");
}
