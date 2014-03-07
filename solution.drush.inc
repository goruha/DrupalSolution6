<?php


/**
 * Implementation of hook_drush_command().
 *
 * @See drush_parse_command() for a list of recognized keys.
 *
 * @return
 *   An associative array describing your command(s).
 */
function solution_drush_command() {
  $items = array();

  $items['solution_install'] = array(
    'description' => "Install solution",
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_ROOT,
    'options' => array(
      'skip-drake' => "Skip drake migrations.",
      'skip-features-revert' => "Skip features revert.",
      'skip-strongarm-revert' => "Skip strongarm revert.",
      'skip-imagecache-revert' => "Skip imagecache revert.",
      'skip-update-db' => "Skip db updates revert.",
      'skip-views-revert' => "Skip views revert.",
      'skip-nodeaccess-rebuild' => "Skip node access rebuild.",
      'skip-translations-update' => "Skip translations update.",
      'skip-webform-revert' => "Skip webform revert.",
      'skip-ubercart-revert' => "Skip ubercart revert.",
    ),
  );

  $items['solution-nodeaccess-rebuild'] = array(
    'description' => "Rebuild nodeaccess",
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_FULL,
  );

  $items['imagecache-presets-revert-all'] = array(
    'description' => "Revert all image cache presets",
    'arguments' => array(
    ),
  );

  return $items;
}

function drush_solution_install() {
  drush_print(dt('------------------------------- Start update process'));

  if (!module_exists('drush_migrate')) {
    drush_print(dt('------------------------------- Enable drake module - START'));
    drush_invoke_process('@self', 'pm-enable', array('drush_migrate'), array('#integrate' => TRUE), TRUE);
    drush_print(dt('------------------------------- Enable drake module - FINISH'));
    drush_invoke_process('@self', 'cc', array('all'), array('#integrate' => TRUE), TRUE);
  }
  
  $skip_drake = drush_get_option('skip-drake');
  if (!$skip_drake) {
      drush_print(dt('------------------------------- Run drake migrate script - START'));
      drush_invoke_process('@self', 'drake', array('--yes'), array('#integrate' => TRUE), TRUE);
      module_list(TRUE, FALSE);
      drush_print(dt('------------------------------- Run drake migrate script - FINISH'));

      drush_print(dt('------------------------------- Clear cache'));
      drush_invoke_process('@self', 'cc', array('all'), array('#integrate' => TRUE), TRUE);
  }

  $skip_update_db = drush_get_option('skip-update-db');
  if (!$skip_update_db) {
    drush_print(dt('------------------------------- Run module updates - START'));
    drush_invoke_process('@self', 'updatedb', array('--yes'), array('#integrate' => TRUE), TRUE);
    drush_print(dt('------------------------------- Run module updates - FINISH'));
  }

  $skip_nodeaccess_rebuild = drush_get_option('skip-nodeaccess-rebuild');
  if (!$skip_nodeaccess_rebuild) {
    drush_print(dt('------------------------------- Run nodeaccess rebuild - START'));
    drush_invoke_process('@self', 'solution-nodeaccess-rebuild', array('--yes'), array('#integrate' => TRUE), TRUE);
    drush_print(dt('------------------------------- Run nodeaccess rebuild - FINISH'));
  }

  $skip_trunslations_update = drush_get_option('skip-translations-update');
  if (!$skip_trunslations_update && module_exists('l10n_update')) {
    drush_print(dt('------------------------------- Revert translations - START'));
    drush_invoke_process('@self', 'l10n-update-refresh', array(), array('#integrate' => TRUE));
    drush_invoke_process('@self', 'l10n-update', array(), array('#integrate' => TRUE));
    drush_print(dt('------------------------------- Revert translations - FINISH'));
  }

  $skip_features_revert = drush_get_option('skip-features-revert');
  if (!$skip_features_revert && module_exists('features')) {
    drush_print(dt('------------------------------- Revert features - START'));
    drush_invoke_process('@self', 'features-revert-all', array('--yes'), array('#integrate' => TRUE));
    drush_print(dt('------------------------------- Revert features - FINISH'));
  }


  $skip_strongarm_revert = drush_get_option('skip-strongarm-revert');
  if (!$skip_strongarm_revert && module_exists('strongarm')) {
    drush_print(dt('------------------------------- Revert variables - START'));
    drush_invoke_process('@self', 'strongarm-revert', array(), array('#integrate' => TRUE));
    drush_print(dt('------------------------------- Revert variables - FINISH'));
  }

//@TODO: add hook for custom calles
  $skip_views_revert = drush_get_option('skip-views-revert');
  if (!$skip_views_revert && module_exists('views')) {
    drush_print(dt('------------------------------- Revert views - START'));
    drush_invoke_process('@self', 'views-revert', array('all'), array('#integrate' => TRUE));
    drush_print(dt('------------------------------- Revert views - FINISH'));
  }

  $skip_imagecache_revert = drush_get_option('skip-imagecache-revert');
  if (!$skip_imagecache_revert && module_exists('imagecache')) {
    drush_print(dt('------------------------------- Revert imagecache presets - START'));
    drush_invoke_process('@self', 'imagecache-presets-revert-all', array(), array('#integrate' => TRUE));
    drush_print(dt('------------------------------- Revert imagecache presets - FINISH'));
  }

  $skip_webform_revert = drush_get_option('skip-webform-revert');
  if (!$skip_webform_revert && module_exists('webforms')) {
    drush_print(dt('------------------------------- Revert webforms - START'));
    drush_invoke_process('@self', 'webforms-revert-all', array(), array('#integrate' => TRUE));
    drush_print(dt('------------------------------- Revert webforms - FINISH'));
  }

  $skip_ubercart_revert = drush_get_option('skip-ubercart-revert');
  if (!$skip_ubercart_revert && module_exists('ubercart')) {
    drush_print(dt('------------------------------- Revert conditional actions - START'));
    drush_invoke_process('@self', 'conditional-actions-revert-all', array(), array('#integrate' => TRUE));
    drush_print(dt('------------------------------- Revert conditional actions - FINISH'));
  }

  drush_print(dt('------------------------------- Clear cache'));
  drush_invoke_process('@self', 'cc', array('all'), array('#integrate' => TRUE));

  if (!$skip_features_revert && module_exists('features')) {
    // we need this because some features do not revert from first run (event races problem)
    drush_print(dt('------------------------------- Revert features CONTROL - START'));
    drush_invoke_process('@self', 'features-revert-all', array('--yes'), array('#integrate' => TRUE));
    drush_print(dt('------------------------------- Revert features CONTROL - FINISH'));

    drush_print(dt('------------------------------- Clear cache'));
    drush_invoke_process('@self', 'cc', array('all'), array('#integrate' => TRUE));
  }
}

function drush_solution_nodeaccess_rebuild() {
  module_load_include('inc', 'node', 'node.admin');
  node_access_rebuild(FALSE);
}

function drush_solution_imagecache_presets_revert_all() {
  $items = imagecache_presets(TRUE);
  foreach ($items as $item) {
    if ($item['storage'] == IMAGECACHE_STORAGE_OVERRIDE) {
      imagecache_preset_delete($item);
      drush_log(dt('Preset @name (ID: @id) was deleted.', array('@name' => $item['presetname'], '@id' => $item['presetid'])), 'ok');
    }
  }
  drush_log(dt('Preset reverted.'), 'ok');
}