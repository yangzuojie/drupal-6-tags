<?php
?>
<select multiple="true" id="tags-term-select">
<?php foreach($data['term_list'] as $term): ?>
<option><?php echo $term->name ?></option>
<?php endforeach; ?>
</select>