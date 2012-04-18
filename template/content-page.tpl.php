<script type="text/javascript">
$(function(){
  $('#tags-tt').tree({
    url: '<?php echo url('tags/content/ajax/tree') ?>',
    onClick:function(node){
      
    }
  });
});
</script>

<div class="easyui-layout" style="width:100%;height:480px;">
  <div region="east" title="&nbsp;" split="true" style="width:480px;">
  </div>
  <div region="west" split="true" title="&nbsp;" style="width:180px;">
    <ul id="tags-tt"></ul>
  </div>

  <div region="center" title="&nbsp;" style="background:#fafafa;overflow:hidden">
    <table class="easyui-datagrid" url="<?php echo url('tags/content/ajax/node') ?>" border="false" fit="true" fitColumns="true">
      <thead>
        <tr>
          <th field="no" width="5"><?php echo t('NO.') ?></th>
          <th field="title" width="80"><?php echo t('Title') ?></th>
        </tr>
      </thead>
    </table>
  </div>

</div>
