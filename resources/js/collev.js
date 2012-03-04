var editor = new Object();

var $console;

log = function(text){
	$console.append('<div>'+text+'</div>');
}

showConsole = function(show){
	if(show)
		$console.show();
	else
		$console.hide();
}

// Array Remove - By John Resig
Array.prototype.remove = function(from, to) {
  var rest = this.slice((to || from) + 1 || this.length);
  this.length = from < 0 ? this.length + from : from;
  return this.push.apply(this, rest);
};

$(function(){

	$console = $("#console");
	var $editorTabs = $("#tabContainer");
	var editorCode = document.getElementById("editorCode");

	var openFiles = new Array();
	var currFile = false;
	var currTheme = 'default';

	//var per = 90/openFiles.length;
	var editPad = 10;
	var tabMargin = 1;
	var tabPadding = 12;
	var maxTabWidth = 120;
	var minTabWidth = 30;

	editor.updateTabs = function(){
		/*
		var fullWidth = $editorTabs.width();
		var availableTabWidth = fullWidth-((leftPad*3) + (openFiles.length*(2*tabMargin)));
		//if((availableTabWidth - (openFiles.length*maxTabWidth) < 0) ){
			maxTabWidth = availableTabWidth/openFiles.length;
		//}
		*/
		var per = 100/openFiles.length;
		for(i=0; i<openFiles.length; i++){
			/*
			if(openFiles.length > 1 && i == openFiles.length-1){
				usedWidth = openFiles[i-1].tab.width()*(openFiles.length-1);
				fullWidth = $editorTabs.width() - (editPad*2);
				openFiles[i].tab.width((((fullWidth-usedWidth)/fullWidth)*100)+'%');
			}
			else{
			*/
				if(openFiles[i])
					openFiles[i].tab.width(per+'%');
			//}
		}
	}

	editor.setCurrent = function(index){
		if(currFile){
			$(currFile.code.getWrapperElement()).removeClass('cm-show');
			$(currFile.code.getWrapperElement()).addClass('cm-hide');
			currFile.tab.removeClass('current');
		}
		currFile = openFiles[index];
		currFile.tab.addClass('current');
		$(currFile.code.getWrapperElement()).removeClass('cm-hide');
		$(currFile.code.getWrapperElement()).addClass('cm-show');
		currFile.code.focus();

      	var his = currFile.code.historySize();
      	if(his['undo'] > 0)
      		qEditor.hasUndo(true);
      	else
      		qEditor.hasUndo(false);

      	if(his['redo'] > 0)
      		qEditor.hasRedo(true);
      	else
      		qEditor.hasRedo(false);

		setTimeout(currFile.code.refresh(), 10);
	}
	editor.close = function(index){

		openFiles[index].tab.remove();
		$(openFiles[index].code.getWrapperElement()).remove();
		openFiles.remove(index);
		qEditor.closeFile(index);
		editor.updateTabs();
	}
		
	editor.openFile = function(qFile){
		var file = new Object();
		file.tab = $('<li class="current"><div class="tab"><span class="tab-title left">'+qFile.name+'['+qFile.mode+']</span><span class="tab-action right"><span class="ico-close"></span></span></div></li>');	
		$editorTabs.append(file.tab);

		file.code = CodeMirror(editorCode, {
	          value: qFile.content,
	          lineNumbers: true,
	          theme: currTheme,
	          onChange: function(){
	          	his = file.code.historySize();
	          	if(his['undo'] == 1)
	          		qEditor.hasUndo(true);
	          	else if(his['undo'] == 0)
	          		qEditor.hasUndo(false);

	          	if(his['redo'] == 1)
	          		qEditor.hasRedo(true);
	          	else if(his['redo'] == 0)
	          		qEditor.hasRedo(false);

	          	qEditor.setFileModified();
	          }
	        });
	    $(file.code.getScrollerElement()).height('100%');
        $(file.code.getScrollerElement()).width('100%');
		
		openFiles[qFile.id] = file;
		editor.updateTabs();
	}

	editor.getFileContents = function(index){
		openFiles[index].code.getFileContents();
	}
	editor.saveFile = function(index){
		qEditor.saveFileContents(index, openFiles[index].code.getValue());
	}

	$("#tabContainer > li").live('click', function(){
		qEditor.switchCurrent($(this).index());
	});

	$("#tabContainer > li").find('span.ico-close').live('click', function(){
		var index = $(this).parent().parent().parent().index();
		qEditor.fileClose(index);
		return false;
	});


	$(window).resize(function(){
	});

	editor.updateTitle = function(index, title){
		openFiles[index].tab.find('.tab-title').html(title);
	}

	editor.undo = function(){
		currFile.code.undo();
	}

	editor.redo = function(){
		currFile.code.redo();
	}

	editor.changeTheme = function(theme)
	{
		for(i=0; i<openFiles.length; i++){
			openFiles[i].code.setOption('theme', theme);
		}
		currTheme = theme;
		editorCode.className = ('cm-s-'+theme);
	}

	editor.showSidebar = function(show){
		var $sidebar = $("#editorSidebar");
		var $codeContainer = $("#editorCodeContainer");
		if(show){
			$sidebar.show();
			$codeContainer.css('left','200px');
		}
		else{
			$sidebar.hide();
			$codeContainer.css('left','0');
		}
	}

	if(qEditor.isSidebarHidden())
		editor.showSidebar(false);

	qEditor.fileOpened.connect(editor.openFile);
	qEditor.fileSave.connect(editor.saveFile);
	qEditor.fileTitleChanged.connect(editor.updateTitle);
	qEditor.currentChanged.connect(editor.setCurrent);
});