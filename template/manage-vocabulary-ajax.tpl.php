<?php
?>
<select multiple="true" id="tags-term-select">
<?php foreach($data['term_list'] as $term): ?>
<option><?php echo $term->name ?></option>
<?php endforeach; ?>
</select>

<div id="tags-term-add-div">
<input type="button" value="<?php echo t('Add') ?>">
</div>