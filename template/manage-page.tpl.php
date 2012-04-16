<?php
?>
<div id="tags-vocabulary-div">
<select id="tags-vocabulary-select">
<option value="0"><?php echo t('- Please choose -') ?></option>
<?php foreach($data['vocabulary_list'] as $vocabulary): ?>
<option value="<?php echo $vocabulary->vid ?>"><?php echo $vocabulary->name ?></option>
<?php endforeach; ?>
</select>
</div>

<div id="tags-term-div">
</div>

<div>
<?php echo $data['tags_table'] ?>
</div>