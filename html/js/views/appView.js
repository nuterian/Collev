define([
  'jquery',
  'underscore', 
  'backbone',
  ], function($, _, Backbone){

    var AppView = Backbone.View.extend({
      initialize:function(){
        this.render();
      },

      render:function(){
        $("#editorView").html("HTML");
      }
    });

    return AppView;
});