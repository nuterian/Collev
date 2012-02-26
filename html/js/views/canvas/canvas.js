define([
  'jquery',
  'underscore', 
  'backbone',
  ], function($, _, Backbone){  

  var Canvas = Backbone.View.extend({
  	tagName: "canvas",
    id: "benchCanvas",

  	initialize: function(){
  		_.bindAll(this, 'render', 'resize');
      $(window).bind('resize', this.resize);
  	},

    render: function(){
      this.resize();
      return this;
    },

    resize: function(){
      $(this.el).attr("height", $(window).height());
      $(this.el).attr("width", $(window).width());
    }
  });

  return Canvas;

});
