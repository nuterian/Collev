define([
  'jquery',
  'underscore', 
  'backbone',
  ], function($, _, Backbone){

  var App = Backbone.Router.extend({
    routes:{
      '': 'home',
    },

    initialize: function(){
    },
      
    home:function () {
      $(funcition(){
        $("#editorView").html("Editor");
      });
    }

  });
  return App;
});