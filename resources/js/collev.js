var editor = new Object();

var $console;
var $sidebar;
var $modeContainer;

log = function(text){
	$console.append('<div>'+text+'</div>');
}

showConsole = function(show){
	if(show)
		$console.show();
	else
		$console.hide();
}

showSidebar = function(show){
	if(show){
		$sidebar.show();
		$modeContainer.css('left','200px');
	}
	else{
		$sidebar.hide();
		$modeContainer.css('left','0');
	}
}

// Array Remove - By John Resig
Array.prototype.remove = function(from, to) {
  var rest = this.slice((to || from) + 1 || this.length);
  this.length = from < 0 ? this.length + from : from;
  return this.push.apply(this, rest);
};

$(function(){

	$console = $("#console");
	$sidebar = $("#sidebar");
	$modeContainer = $("#modeContainer");

	showSidebar(APP.isSidebarVisible);
	APP.toggleSidebar.connect(showSidebar);

	var $editorTabs = $("#tabContainer");
	var $editorBar = $sidebar.find("#editorBar");
	var $editorOpenFiles = $editorBar.find("#editorOpenFiles > ul");
	var $editorStatus = $("#editorStatus");
	var editorCode = document.getElementById("editorCode");

	var openFiles = new Array();
	var currFile = false;
	var currTheme = 'default';
	var currMode = 'text/plain';

	//var per = 90/openFiles.length;
	var editPad = 10;
	var tabMargin = 1;
	var tabPadding = 12;
	var maxTabWidth = 120;
	var minTabWidth = 30;

	var init = true;

	editor.updateTabs = function(){
		var per = 100/openFiles.length;
		for(i=0; i<openFiles.length; i++){
				if(openFiles[i])
					openFiles[i].tab.width(per+'%');
		}
	}

	editor.setCurrent = function(){
		if(currFile){
			$(currFile.code.getWrapperElement()).removeClass('cm-show');
			$(currFile.code.getWrapperElement()).addClass('cm-hide');
			currFile.tab.removeClass('current');
			currFile.side.removeClass('current');
		}
		currFile = this;
		currFile.tab.addClass('current');
		currFile.side.addClass('current');
		$(currFile.code.getWrapperElement()).removeClass('cm-hide');
		$(currFile.code.getWrapperElement()).addClass('cm-show');
		currFile.code.focus();

      	var his = currFile.code.historySize();
      	if(his['undo'] > 0) qEditor.hasUndo(true);
      	else qEditor.hasUndo(false);

      	if(his['redo'] > 0) qEditor.hasRedo(true);
      	else qEditor.hasRedo(false);

		setTimeout(currFile.code.refresh(), 10);
		$("#modeName", $editorStatus).html(this.f.mode.typeName);

	}
	editor.closeFile = function(){
		this.tab.remove();
		this.side.remove();
		$(this.code.getWrapperElement()).remove();
		for(i=0; i<openFiles.length; i++){
			if(openFiles[i] == this){
				openFiles.remove(i);
				break;
			}
		}
		editor.updateTabs();
		if(openFiles.length == 0){
			$("#modeName", $editorStatus).html('&nbsp;');
		}
	}

	editor.saveFile = function(index){
		this.f.content = this.code.getValue();
		this.undoPos = this.code.historySize().undo;
	}

	editor.changeMode = function(){
		this.code.setOption("mode", this.f.type.mimeName);
	}

	editor.changeName = function(){
		this.tab.find('.tab-title').html(this.f.name);
		this.side.find('.side-name').html(this.f.name);
	}

	editor.changeMode = function(){
		this.code.setOption("mode", this.f.mode.mimeName);
		if(this == currFile)
			$("#modeName", $editorStatus).html(this.f.mode.typeName);
	}

	var initCode;

	createCodemirror = function(el, theme, mode, value){
		code = CodeMirror(editorCode, {
			  value: value,
	          lineNumbers: true,
	          theme: theme,
	          mode: mode
	        }); 

		$(code.getScrollerElement()).height('100%');
        $(code.getScrollerElement()).width('100%');
        setTimeout(code.refresh(), 10);
        return code;
	}

	editor.init = function(){
		initCode = createCodemirror(editorCode, currTheme, currMode);
		initCode.setOption("onChange", function(){if(initCode)qEditor.newFile();});
		initCode.focus();
	}

	editor.setModified = function(modified){
		if(modified){
	        this.tab.addClass('modified');
	        this.side.addClass('modified');	
		}
		else{
			this.tab.removeClass('modified');
	        this.side.removeClass('modified');
		}
	}

	editor.open = function()
	{
		var file = new Object();
		file.f = window.file;

		// Connect all Signals
		file.f.isCurrent.connect(file,editor.setCurrent);
        file.f.wasModified.connect(file,editor.setModified);
        file.f.nameChanged.connect(file, editor.changeName);
        file.f.modeChanged.connect(file, editor.changeMode);
        file.f.closed.connect(file, editor.closeFile);
        file.f.save.connect(file, editor.saveFile);


		file.tab = $('<li class="current"><div class="tab"><span class="tab-title">'+file.f.name+'</span><span class="tab-action"></span></div></li>');
		file.side = $('<li class="current"><span class="side-action"></span><span class="side-name">'+file.f.name+'</span></li>');
		$editorTabs.append(file.tab);
		$editorOpenFiles.append(file.side);
		if(initCode){
			file.code = initCode;
			initCode = false;
			if(!file.f.isNew){
				file.code.setOption("theme", currTheme);
				file.code.setOption("mode", file.f.type.mimeName);
				file.code.setValue(file.f.content);
				file.code.clearHistory();
			}
			else{
				if(file.code.getValue() != ''){
	          		file.f.isModified = true;
	          	}
			}
		}
		else{
			file.code = createCodemirror(editorCode, currTheme, file.f.type.mimeName, file.f.content);
		}

		file.undoPos = 0;

		file.code.setOption("onChange", function(){
	          	his = file.code.historySize();
	          	console.log('Change! : '+his['undo']+' => '+file.undoPos);
	          	if(his['undo'] == (file.undoPos+1) || his['undo'] == (file.undoPos-1)){
	          		qEditor.hasUndo(true);
	          		file.f.isModified = true;
	          	}
	          	else if(his['undo'] == file.undoPos){
	          		qEditor.hasUndo(false);
	          		file.f.isModified = false;
	          	}
	          		
	          	if(his['redo'] == 1)
	          		qEditor.hasRedo(true);
	          	else if(his['redo'] == 0)
	          		qEditor.hasRedo(false);
	          });

        openFiles.push(file);

        file.tab.click(function(){file.f.isCurrent(true);});
        file.side.click(function(){file.f.isCurrent(true);});
        file.tab.find('span.tab-action').click(function(){file.f.closing();return false});
        file.side.find('span.side-action').click(function(){file.f.closing();return false});
        file.f.isCurrent();
        editor.updateTabs();
	}

	editor.undo = function(){
		currFile.code.undo();
	}

	editor.redo = function(){
		currFile.code.redo();
	}

	editor.changeTheme = function(theme)
	{
		console.log("Chaning Theme...");
		if(initCode)
			initCode.setOption('theme', theme);
		else{
			for(i=0; i<openFiles.length; i++){
				openFiles[i].code.setOption('theme', theme);
			}
		}
		currTheme = theme;
		editorCode.className = ('cm-s-'+theme);
	}

	qEditor.fileOpened.connect(editor.open);
	editor.init();
});