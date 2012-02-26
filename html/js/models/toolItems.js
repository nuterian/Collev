define([
  'backbone',
  ], function(Backbone){

	ToolItem = Backbone.Model.extend({
		defaults:{
			name: '',
			icoClass: ''
		}	
	});

	ToolItems = Backbone.Collection.extend({
		model: ToolItem
	});

  return ToolItems;
});
  