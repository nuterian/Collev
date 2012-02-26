define([
  'jquery',
  'underscore', 
  'backbone',
  'models/toolItems'
  ], function($, _, Backbone, ToolItems){

	var tools = [
		{
			name: "Sources and Sinks",
			icoClass: "tool-sources"
		},
		{
			name: "Generators",
			icoClass: "tool-generator"
		},
		{
			name: "Reporters",
			icoClass: "tool-reporter"
		},
		{
			name: "Measurement",
			icoClass: "tool-measurement"
		},
		{
			name: "Signalling",
			icoClass: "tool-signalling"
		},
	];
	

	ToolView = Backbone.View.extend({
		tagName: "div",
		className: "toolItem",

		initialize: function(){
			_.bindAll(this, "render");
		},

		render: function(){
			$(this.el).html("<div class='"+this.model.get('icoClass')+"'></div>");
			return this;
		}
	});

	ToolPanelView = Backbone.View.extend({

    id:"toolPanel",

		initialize: function(){
			_.bindAll(this, 'render');
			this.collection = new ToolItems(tools);
		},

		render: function(){
			$(this.el).empty();
			_(this.collection.models).each(function(item){
				var toolView = new ToolView({model:item});
				$(this.el).append(toolView.render().el);
			}, this);
      return this;
		}
	});

	return ToolPanelView;
});