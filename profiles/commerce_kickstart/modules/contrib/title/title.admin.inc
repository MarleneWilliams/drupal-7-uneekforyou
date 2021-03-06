<?php

/**
 * @file
 * Admin page callbacks for the Title module.
 */

/**
 * Provide settings to enable title field.
 */
function title_form_field_ui_overview(&$form, &$form_state) {
  $entity_info = entity_get_info($form['#entity_type']);

  if (!empty($entity_info['field replacement'])) {
    $field_replacement_info = $entity_info['field replacement'];
    $admin_path = _field_ui_bundle_admin_path($form['#entity_type'], $form['#bundle']);
    $form['fields']['#header'][6]['colspan'] += 1;

    foreach (element_children($form['fields']) as $field_name) {
      if (isset($field_replacement_info[$field_name])) {
        $form['fields'][$field_name]['field_replacement'] = array(
          '#type' => 'link',
          '#title' => t('replace'),
          '#href' => $admin_path . '/fields/replace/' . $field_name,
          '#options' => array('attributes' => array('title' => t('Replace %field with a field instance.', array('%field' => $field_name)))),
        );
      }
      else {
        $form['fields'][$field_name]['field_replacement'] = array();
      }
    }
  }
}

/**
 * Generate a field replacement form.
 */
function title_field_replacement_form($form, $form_state, $entity_type, $bundle, $field_name) {
  $bundle_name = field_extract_bundle($entity_type, $bundle);
  $entity_info = entity_get_info($entity_type);
  $info = $entity_info['field replacement'][$field_name];
  $instance = field_info_instance($entity_type, $info['field']['field_name'], $bundle_name);
  $enabled = !empty($instance);

  $form['#entity_type'] = $entity_type;
  $form['#bundle'] = $bundle_name;
  $form['#field_name'] = $field_name;

  $form['enabled'] = array(
    '#type' => 'checkbox',
    '#title' => t('Replace %field with a field instance.', array('%field' => $field_name)),
    '#description' => t('If this is enabled the %field legacy field will be replaced with a regular field and will disappear from the <em>Manage fields</em> page. It will get back if the replacing field instance is deleted.', array('%field' => $field_name)),
    '#default_value' => $enabled,
    '#disabled' => $enabled,
  );

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array('#type' => 'submit', '#value' => t('Save settings'));
  return $form;
}

/**
 * Process field replacement form subissions.
 */
function title_field_replacement_form_submit($form, &$form_state) {
  if ($form_state['values']['enabled'] != $form['enabled']['#default_value']) {
    if (title_field_replacement_toggle($form['#entity_type'], $form['#bundle'], $form['#field_name'])) {
      drupal_set_message(t('%field replaced with a field instance.', array('%field' => $form['#field_name'])));
      title_field_replacement_batch_set($form['#entity_type'], $form['#bundle'], $form['#field_name']);
    }
    else {
      drupal_set_message(t('Field replacement removed.'));
    }
  }
  $form_state['redirect'] = _field_ui_bundle_admin_path($form['#entity_type'], $form['#bundle']) . '/fields';
}

/**
 * Form settings for automated title_field attachement.
 */
function title_admin_settings_form() {
  foreach (entity_get_info() as $entity_type => $entity) {
    if (empty($entity['field replacement'])) {
      continue;
    }

    foreach (array_keys($entity['field replacement']) as $replacement) {
      $options[$entity_type . ':' . $replacement] = $entity['label'] ." - ". ucfirst($replacement);
    }
  }

  $form['title_auto_attach'] = array(
    '#type' => 'select',
    '#title' => t('Auto attach replacement field'),
    '#options' => $options,
    '#multiple' => TRUE,
    '#default_value' => variable_get('title_auto_attach', array()),
    '#description' => t('Enable to auto-attach a replacement field when a bundle is created.'),
  );

  return system_settings_form($form);
}
