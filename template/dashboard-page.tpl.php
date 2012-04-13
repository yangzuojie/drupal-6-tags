<?php
?>
<script type="text/javascript">
  $(function(){
    $('#taxonomy-tt').tree({
      data: [<?php echo json_encode($data['data_list']) ?>],
      dnd: true,
      animate: true,
      onClick:function(node){
        $(this).tree('beginEdit', node.target);
      },
      onAfterEdit:function(node){
        var parent = $('#taxonomy-tt').tree('getParent', node.target).id;
        $.post( '<?php echo url('tags/save') ?>', {id: node.id, parent: parent, text: node.text}, function(data){
            node.id = data.id;
            $('#taxonomy-tt').tree('update', node);
        }, 'json');
      },
      onContextMenu: function(e, node){
        e.preventDefault();
        $('#taxonomy-tt').tree('select', node.target);
        $('#mm').menu('show', {
          left: e.pageX,
          top: e.pageY
        });
      },
      onDrop: function(targetNode, source, point){
        var targetId = $('#taxonomy-tt').tree('getNode', targetNode).id;
        $.post( '<?php echo url('tags/drop') ?>', {id: source.id, target_id: targetId, point: point}, function(data){
        }, 'json');
      }
    });
  });
  function append(){
    var node = $('#taxonomy-tt').tree('getSelected');
    $('#taxonomy-tt').tree('append',{
      parent: (node?node.target:null),
      data:[{
        text:'new1',
        checked:true
      }]
    });
  }
  function remove(){
    var node = $('#taxonomy-tt').tree('getSelected');
    var parent = $('#taxonomy-tt').tree('getParent', node.target).id;
    $.post( '<?php echo url('tags/remove') ?>', {id: node.id, parent: parent}, function(data){
      $('#taxonomy-tt').tree('remove', node.target);
    }, 'json');
  }
</script>

<ul id="taxonomy-tt"></ul>

<div id="mm" class="easyui-menu" style="width:120px;">
  <div onclick="append()" iconCls="icon-add">Append</div>
  <div onclick="remove()" iconCls="icon-remove">Remove</div>
</div>