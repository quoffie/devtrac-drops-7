<?php
class DevtracImportsSectorIcons extends XMLMigration {
  public function __construct() {
    parent::__construct();
    $this->description = t('Example migration of media images');

    $this->map = new MigrateSQLMap($this->machineName,
      array(
        'code' => array(
          'type' => 'varchar',
          'length' => 255,
          'not null' => TRUE,
          'description' => 'Image filename',
        )
      ),
      MigrateDestinationMedia::getKeySchema()
    );

    // Source fields available in the XML file.
    $fields = array(
      'code' => t('sector code'),
      'filename' => t('Image filename, relative to the source directory'),
      'description' => t('Description of the image'),
      'name' => t('Name of Sector Code'),
    );

    // Our test data is in an XML file
    $xml_folder = drupal_get_path('module', 'devtrac_imports');
    $items_url = $xml_folder . '/data/sectors.xml';
    //$items_url = 'http://codelists103.archive.iatistandard.org/data/codelist/Sector/version/1.0/lang/en.xml';
    $item_xpath = '/codelist/Sector';
    $item_ID_xpath = 'code';
    $items_class = new MigrateItemsXML($items_url, $item_xpath, $item_ID_xpath);
    $this->source = new MigrateSourceMultiItems($items_class, $fields);

    // In the simplest case, just pass the media type.
    $this->destination = new MigrateDestinationMedia('image');

    // The source images are in a local directory - specify the parent.
    $this->addFieldMapping('source_dir')
         ->defaultValue(path_to_theme() . '/icons/sectors');    // The 'value' of the media destination is mapped to the source field
    // representing the media itself - in this case, a filename relative to
    // source_dir.
    $this->addFieldMapping('value', 'filename');
         //->xpath('code');
    // Fields on the entity can be mapped in the usual way.
//    $this->addFieldMapping('field_image_description', 'description')
//         ->xpath('description');

    $this->addFieldMapping('uid')
         ->defaultValue(1);
    $this->addFieldMapping('field_file_image_title_text', 'name')->xpath('name');
    $this->addFieldMapping('field_file_image_alt_text', 'name')->xpath('name');
    
    $this->addUnmigratedDestinations(array('field_image_description:format',
      'field_image_description:language', 'destination_dir', 'destination_file',
      'file_replace', 'preserve_files', 'timestamp'));
    if (module_exists('path')) {
      $this->addUnmigratedDestinations(array('path'));
    }
  }

  public function prepareRow($row) {
    // Always include this fragment at the beginning of every prepareRow()
    // implementation, so parent classes can ignore rows.
    if (parent::prepareRow($row) === FALSE) {
      return FALSE;
    }

    $row->filename = $row->code . '.png';
    $fieldmappings = $this->getFieldMappings();
    $source_dir = $fieldmappings['source_dir']->getDefaultvalue();
    $file_full_path = DRUPAL_ROOT . '/' . $source_dir . '/' . $row->filename;
    if(!file_exists($file_full_path)) {
      return FALSE;
    }

    return TRUE;
  }  
}
