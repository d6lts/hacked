<?php


function hacked_reports_hacked_details($project) {
  // Send the results to the theme function:
  $out = theme('hacked_detailed_report', array('project' => $project->compute_details()));

  return $out;
}

/**
 * Theme project status report.
 *
 * @ingroup themeable
 */
function theme_hacked_detailed_report($variables) {
  // Have a context for translation.
  $translation_context = array('context' => 'Hacked');
  $project = $variables['project'];
  $rows = array();
    foreach ($project['files'] as $file => $status) {
      switch ($status) {
        case HACKED_STATUS_UNHACKED:
          $class = 'ok';
          $icon = theme('image', array('path' => 'misc/watchdog-ok.png', 'alt' => t('Unchanged', array(), $translation_context), 'title' => t('Unchanged', array(), $translation_context)));
          break;
        case HACKED_STATUS_DELETED:
          $class = 'error';
          $icon = theme('image', array('path' => 'misc/watchdog-error.png', 'alt' => t('Changed', array(), $translation_context), 'title' => t('Changed', array(), $translation_context)));
          break;
        case HACKED_STATUS_HACKED:
          $class = 'warning';
          $icon = theme('image', array('path' => 'misc/watchdog-warning.png', 'alt' => t('Deleted', array(), $translation_context), 'title' => t('Deleted', array(), $translation_context)));
          break;
        case HACKED_STATUS_PERMISSION_DENIED:
          $class = 'warning';
          $icon = theme('image', array('path' => 'misc/watchdog-warning.png', 'alt' => t('Permission Denied', array(), $translation_context), 'title' => t('Permission Denied', array(), $translation_context)));
          break;
        case HACKED_STATUS_UNCHECKED:
        default:
          $class = 'warning';
          $icon = theme('image', array('path' => 'misc/watchdog-warning.png', 'alt' => t('Unchecked', array(), $translation_context), 'title' => t('Unchecked', array(), $translation_context)));
          break;
      }

      $row = '<div class="version-status">';
      switch ($status) {
        case HACKED_STATUS_UNHACKED:
          $row .= t('Unchanged', array(), $translation_context);
          break;
        case HACKED_STATUS_HACKED:
          $row .= '<span class="not-current">' . t('Changed!', array(), $translation_context) . '</span>';
          break;
        case HACKED_STATUS_DELETED:
          $row .= '<span class="">' . t('Deleted', array(), $translation_context) . '</span>';
          break;
        case HACKED_STATUS_PERMISSION_DENIED:
          $row .= '<span class="">' . t('Permission Denied', array(), $translation_context) . '</span>';
          break;
        case HACKED_STATUS_UNCHECKED:
        default:
          $row .= '<span class="">' . t('Unchecked', array(), $translation_context) . '</span>';
          break;
      }
      $row .= '<span class="icon">' . $icon . '</span>';
      $row .= "</div>\n";

      $row .= '<div class="project">';
      if (user_access('view diffs of changed files') && module_exists('diff') && ($status != HACKED_STATUS_UNHACKED) && !empty($project['diffable'][$file])) {
        $row .= l($file, 'admin/reports/hacked/' . $project['name'] . '/diff/' . $file);
      }
      else {
        $row .= check_plain($file);
      }
      $row .= "</div>\n";

      $row .= "</div>\n";

      $rows[] = array(
        'class' => array($class),
        'data' => array($row),
      );
    }
    drupal_add_css(drupal_get_path('module', 'update') . '/update.css');
    return theme('table', array('rows' => $rows, 'attributes' => array('class' => 'update')));
}
