var editor = new Object();

$(function(){
	console.log('Initialized Collev');	

	var $editorTabs = $("#tabContainer");
	var editorCode = document.getElementById("editorCode");

	var openFiles = new Array();
	var currFile = false;

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
			if(openFiles.length > 1 && i == openFiles.length-1){
				usedWidth = openFiles[i-1].tab.width()*(openFiles.length-1);
				fullWidth = $editorTabs.width() - (editPad*2);
				openFiles[i].tab.width((((fullWidth-usedWidth)/fullWidth)*100)+'%');
			}
			else{
				openFiles[i].tab.width(per+'%');
			}
		}
	}

	editor.setCurrent = function(index){
		if(currFile){
			$(currFile.code.getWrapperElement()).removeClass('cm-show');
			$(currFile.code.getWrapperElement()).addClass('cm-hide');
			currFile.tab.removeClass('current');
		}
		currFile = openFiles[index];
		console.log(currFile.tab);
		currFile.tab.addClass('current');
		$(currFile.code.getWrapperElement()).removeClass('cm-hide');
		$(currFile.code.getWrapperElement()).addClass('cm-show');
		currFile.code.focus();
		setTimeout(currFile.code.refresh(), 10);
	}
		
	editor.openFile = function(qFile){
		console.log('Opening File - '+qFile.name);
		var file = new Object();
		file.tab = $('<li class="current"><div class="tab"><div class="tab-title left">'+qFile.name+'</div><div class="tab-action right">');	
		file.tab.attr('data-id', qFile.id);
		$editorTabs.append(file.tab);

		file.code = CodeMirror(editorCode, {
	          value: qFile.content,
	          lineNumbers: true,
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
		console.log(qFile.id);
		//editor.setCurrent(file);
		editor.updateTabs();
		console.log('File Opened');
	}

	editor.getFileContents = function(index){
		openFiles[index].code.getFileContents();
	}
	editor.saveFile = function(index){
		qEditor.saveFileContents(index, openFiles[index].code.getValue());
	}

	$("#tabContainer > li").live('click',function(){
		qEditor.changeCurrent(parseInt($(this).attr('data-id')));
	});

	$(window).resize(function(){
		console.log('resizing');
		//editor.updateTabs();
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

	qEditor.fileOpened.connect(editor.openFile);
	qEditor.fileSave.connect(editor.saveFile);
	qEditor.fileTitleChanged.connect(editor.updateTitle);
	qEditor.currentChanged.connect(editor.setCurrent);
});