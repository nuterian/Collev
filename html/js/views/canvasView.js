define([
  'jquery',
  'underscore', 
  'backbone',
  'views/canvas/canvas',
  'views/canvas/toolPanel',
  'views/canvas/toolbar',
  'views/canvas/sidePanel',
  ], function($, _, Backbone, Canvas, ToolPanelView, Toolbar, SidePanelView){

  var CanvasView = Backbone.View.extend({
    id: "canvasView",

    initialize: function(){
      this.canvas = new Canvas();
      this.toolPanel = new ToolPanelView();
      console.log('CanvasView: '+this.model);
      this.toolbar = new Toolbar({model:this.model});
      this.sidePanel = new SidePanelView({model:this.model});
    },

    render: function(){
      $(this.el).append(this.canvas.render().el);  
      $(this.el).append(this.toolPanel.render().el);
      $(this.toolPanel.el).css('left', '-50px');
      $(this.toolPanel.el).animate({left: 0});
      $(this.el).append(this.sidePanel.render().el);
      $(this.toolbar.el).show();
      return this;
    },

    unrender: function(){
      $(this.el).empty();
      $(this.toolbar.el).hide();
    }

  });

  return CanvasView;
});