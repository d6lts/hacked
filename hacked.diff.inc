<?php


/**
 * Shows a diff report for a specific file in a project.
 *
 * @param $project
 *   The hackedProject instance.
 * @param $file
 *   The file to diff within the project.
 *
 * @return
 *   If the user is allowed to view the diff report, then a HTML diff report,
 *   otherwise a HTML error string.
 *
 */
function hacked_reports_hacked_diff($project, $file) {

  if (!module_exists('diff')) {
    return t('The diff module is required to use this feature.');
  }

  $project->identify_project();

  // Find a better way to do this:
  $breadcrumb = array(
    l('Home', '<front>'),
    l('Administer', 'admin'),
    l('Reports', 'admin/reports'),
    l('Hacked', 'admin/reports/hacked'),
    l($project->title(), 'admin/reports/hacked/' . $project->name),
  );
  drupal_set_breadcrumb($breadcrumb);

  if ($project->file_is_diffable($file)) {
    return hacked_diff_changed($project, $file);
  }
  return t('Cannot hash binary file or file not found: %file', array('%file' => $file));
}

/**
 * Generate a diff report for a specific file in a project.
 *
 * @param $project
 *   The hackedProject instance.
 * @param $file
 *   The file to diff within the project.
 *
 * @return
 *   A HTML diff report.
 */
function hacked_diff_changed($project, $file) {
  $original_file = $project->file_get_location('remote', $file);
  $installed_file = $project->file_get_location('local', $file);

  $hasher = hacked_get_file_hasher();

  $output = theme('table', array(
                  'header' => array(t('Original'), '', t('Current'), ''),
                  'rows' => diff_get_rows($hasher->fetch_lines($original_file), $hasher->fetch_lines($installed_file), TRUE)));
  return $output;
}
