define([
  'jquery',
  'underscore', 
  'backbone',
  'text!templates/toolbar.html'
  ], function($, _, Backbone, toolbarTemplate){  
/*
  var Condition = Backbone.Model.extend({});
  var Conditions = Backbone.Collection.extend({model:Condition});

  var ConditionView = Backbone.View.extend({
    tagName: 'li',
    className: 'clearfix',
    render: function(){
      $(this.el).html('<div class="column">'+this.model.get('status')+'</div><div class="column last">'+this.model.get('desc')+'</div>');
      return this;
    }
  });

  var Progress = Backbone.View.extend({
    id: "solutionProgress",

    initialize: function(){
      _.bindAll(this, 'render');
    },

    render: function(){
      $(this.el).empty();
      $(this.el).append('<div class="progress-container"><div class="progress"></div></div>');
      $mainProgress = $('.progress', this.el);
      $conditionList = $('<ul class="drop-list"></ul>');

      var done = 0;
      _(this.collection.models).each(function(condition){
        var conditionView = new ConditionView({model:condition});
        if(condition.get('status') == true){done++;}
        $conditionList.append(conditionView.render().el);
      }, this);
      var per = (done/this.collection.length)*100;
      $mainProgress.css('width', per+'%'); 

      $(this.el).append($conditionList);
      return this;
    }
  });

*/

  var Toolbar = Backbone.View.extend({
    el: $("#toolbar"),

    template: _.template(toolbarTemplate),

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

  return Toolbar;

});
