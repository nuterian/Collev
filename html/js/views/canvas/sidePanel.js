define([
  'jquery',
  'underscore', 
  'backbone',
  'text!templates/sidePanel.html'
  ], function($, _, Backbone, sidePanelTemplate){ 

  var SidePanelView = Backbone.View.extend({
    id:"sidePanel",

    template: _.template(sidePanelTemplate),

    initialize: function(){
      if(this.model){
        this.render();
      }
    },

    render: function(){
      $(this.el).html(this.template(this.model.toJSON()));
      return this;
    }
  });

  return SidePanelView;

});